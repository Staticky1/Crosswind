#include "MissionBriefingWindow.h"
#include "Core/CrosswindApp.h"
#include "Core/EnumHelpers.h"

MissionBriefingWindow::MissionBriefingWindow(std::shared_ptr<MissionContext> context)
	:CrosswindWindow(LOCAL("MissionBriefing"))
	,m_Context(context)
	,m_CampaignData(context->campaignData)
	,m_SelectedPlayerPilotIndex(context->selectedPlayerPilotIndex)
	,m_Mission(context->mission)
{
	m_WindowFlags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar;
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 50, ImGui::GetMainViewport()->WorkSize.y - 240);
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));

}

void MissionBriefingWindow::RenderContents()
{
	// Must be called from somewhere before rendering
	ImGui::OpenPopup("MissionBriefing");

	// Get main viewport (usually the application window)
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();


	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 50, ImGui::GetMainViewport()->WorkSize.y - 140);

	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));


	// Calculate center position
	ImVec2 popupPos = ImVec2(
		main_viewport->Pos.x + (main_viewport->Size.x - m_WindowSize.x) * 0.5f,
		main_viewport->Pos.y + (main_viewport->Size.y - m_WindowSize.y) * 0.5f
	);

	ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);    // Or ImGuiCond_Once
	ImGui::SetNextWindowSize(m_WindowSize, ImGuiCond_Always);  // Or ImGuiCond_Once
	
	if (ImGui::BeginPopupModal("MissionBriefing", nullptr, m_WindowFlags))
	{		
		switch (briefingStage)
		{
		case 0: //weather and briefing
		{
			ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
			ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(m_Mission.missionPlan.missionType.name.c_str()).x) * 0.5f);
			ImGui::AlignTextToFramePadding();
			ImGui::Text(m_Mission.missionPlan.missionType.name.c_str());
			ImGui::PopFont();

			ImVec2 TableSize = ImVec2(m_WindowSize.x - m_WindowSize.x * 0.3f, 0.f);
			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			ImGui::TextWrapped(m_Mission.missionPlan.missionType.description.c_str());

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 50));
			ImGui::SetCursorPosX((m_WindowSize.x - (TableSize.x + ImGui::GetStyle().ItemSpacing.x)) * 0.5f);
			ImGui::BeginChild("##WeatherWindowLeft", ImVec2(TableSize.x * 0.5f, 300), false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
			ImGui::Text(LOCAL("MissionWeather"));
	
			ImGui::Text(LOCAL("WeatherAirTemp")); ImGui::SameLine();
			ImGui::Text((Core::FloatToString(m_CampaignData.CurrentWeather.airTemp,1) + "c").c_str());

			ImGui::Text(LOCAL("WeatherAirPressure")); ImGui::SameLine();
			ImGui::Text((Core::FloatToString(m_CampaignData.CurrentWeather.airPressure,1) + " Mmhg").c_str());


			std::string cloudCoverType;
			if (Core::StringContains(m_CampaignData.CurrentWeather.cloudConfig,"00"))
			{
				cloudCoverType = LOCAL("WeatherClear");
			}
			else if (Core::StringContains(m_CampaignData.CurrentWeather.cloudConfig, "01"))
			{
				cloudCoverType = LOCAL("WeatherLight");
			}
			else if (Core::StringContains(m_CampaignData.CurrentWeather.cloudConfig, "02"))
			{
				cloudCoverType = LOCAL("WeatherMedium");
			}
			else if (Core::StringContains(m_CampaignData.CurrentWeather.cloudConfig, "03"))
			{
				cloudCoverType = LOCAL("WeatherHeavy");
			}
			else if (Core::StringContains(m_CampaignData.CurrentWeather.cloudConfig, "04"))
			{
				cloudCoverType = LOCAL("WeatherOvercast");
			}
			ImGui::Text(LOCAL("WeatherCloudCover")); ImGui::SameLine();
			ImGui::Text(cloudCoverType.c_str());

			ImGui::Text(LOCAL("WeatherCloudBase")); ImGui::SameLine();
			ImGui::Text(std::to_string(m_CampaignData.CurrentWeather.cloudBase).c_str());
			ImGui::Text(LOCAL("WeatherCloudTop")); ImGui::SameLine();
			ImGui::Text(std::to_string(m_CampaignData.CurrentWeather.cloudTop).c_str());
			ImGui::Text(LOCAL("WeatherPrecipitationAmount")); ImGui::SameLine();
			std::string rainText;
			if (m_CampaignData.CurrentWeather.rainType == 0)
			{
				rainText = "None";
			}
			else
			{
				rainText = Core::GetWeightLabel(m_CampaignData.CurrentWeather.rainAmount, 0, 9);
			}
			ImGui::Text(LOCAL("Rain" + rainText));
			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("##WeatherWindowRight", ImVec2(TableSize.x * 0.5f, 300), false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

			DisplayWindProfile(m_CampaignData.CurrentWeather.wind);
			ImGui::Text(LOCAL("WeatherSeaState")); ImGui::SameLine();
			ImGui::Text(std::to_string(m_CampaignData.CurrentWeather.seaState).c_str());
			ImGui::EndChild();

			ImGui::Dummy(ImVec2(0, 50));
			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			//select briefing
			
			switch (m_Mission.missionPlan.missionType.type)
			{
			case EMissionType::CAP_HIGH:
			case EMissionType::CAP_LOW:
				ImGui::BulletText(LOCAL("MissionCAP01"));
				ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
				ImGui::BulletText(LOCAL("MissionCAP02"));
				ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
				ImGui::BulletText(LOCAL("MissionCAP03"));
				break;
			default:
				break;
			}	
			ImGui::Dummy(ImVec2(0, 50));
			ImGui::SetCursorPosX((m_WindowSize.x - (ImGui::CalcTextSize(LOCAL("Button_Cancel")).x + ImGui::CalcTextSize(LOCAL("Button_Next")).x + (ImGui::GetStyle().FramePadding.x * 4) + ImGui::GetStyle().ItemSpacing.x)) * 0.5f);
			if (ImGui::Button(LOCAL("Button_Cancel")))
			{
				CurrentAction = EWindowActions::CLOSE;
			}
			ImGui::SameLine();
			if (ImGui::Button(LOCAL("Button_Next")))
			{
				NextStage();
			}
			break;
		}
		case 1: //pilots and loadout
		{

			std::vector<PilotData>& assignedPilots = m_Mission.AssignedPilots;
			std::vector<SquadronAircraft>& assignedAircraft = m_Mission.assignedAircraft;

			ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
			ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("MissionPilotsAndAircraft")).x) * 0.5f);
			ImGui::Text(LOCAL("MissionPilotsAndAircraft"));
			ImGui::PopFont();
			ImGui::Separator();

			if (ImGui::BeginTable("MissionPilotTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
				ImGui::TableSetupColumn("Move");
				ImGui::TableSetupColumn("Order");
				ImGui::TableSetupColumn("Pilot");
				ImGui::TableSetupColumn("Aircraft");
				ImGui::TableSetupColumn("Loadout");
				ImGui::TableSetupColumn("Remove");
				ImGui::TableHeadersRow();


				for (int i = 0; i < assignedPilots.size(); ++i) {

					std::vector<const char*> loadoutNames;
					for (const auto& loadout : assignedAircraft[i].availableLoadouts) {
						loadoutNames.push_back(loadout.name.c_str());
					}
					
					ImGui::PushID(i);
					
					ImGui::TableNextRow();
					//Move
					ImGui::TableSetColumnIndex(0);
					if (ImGui::ArrowButton(("Up##" + std::to_string(i)).c_str(), ImGuiDir_Up)) {
						if (i > 0) std::swap(assignedPilots[i], assignedPilots[i - 1]);
					}
					ImGui::SameLine();
					if (ImGui::ArrowButton(("Down##" + std::to_string(i)).c_str(), ImGuiDir_Down)) {
						if (i < assignedPilots.size() - 1) std::swap(assignedPilots[i], assignedPilots[i + 1]);
					}

					//Position
					ImGui::TableSetColumnIndex(1);
					std::string orderText;
					if (i > 0)
					{
						std::unordered_map<std::string, std::string> nameMap = {
								{"pos", std::to_string(i)}
						};
						orderText = LOCAL_FORMAT("Wingman", false, nameMap);
					}
					else
					{
						orderText = LOCAL("FlightLead");
					}
					ImGui::Text(orderText.c_str());

					//Pilot name
					ImGui::TableSetColumnIndex(2);
					if (assignedPilots[i].bIsPlayerPilot) ImGui::PushStyleColor(ImGuiCol_Text, PLAYERCOLOUR);
					ImGui::Text((assignedPilots[i].Rank.abbrevation + " " + assignedPilots[i].PilotName).c_str());
					if (assignedPilots[i].bIsPlayerPilot) ImGui::PopStyleColor();
					if (ImGui::IsItemHovered())
					{
						// Load texture if not already loaded
						if (!assignedPilots[i].IconTexture && !assignedPilots[i].IconPath.empty())
						{
							assignedPilots[i].IconTexture = std::make_shared<Walnut::Image>(assignedPilots[i].IconPath); // Replace with your actual loader if different
						}

						if (assignedPilots[i].IconTexture && assignedPilots[i].IconTexture->GetWidth() > 0 && assignedPilots[i].IconTexture->GetHeight() > 0)
						{
							ImGui::BeginTooltip();
							float maxWidth = 120.0f; // limit tooltip width
							float aspectRatio = (float)assignedPilots[i].IconTexture->GetHeight() / assignedPilots[i].IconTexture->GetWidth();
							ImVec2 imageSize = ImVec2(maxWidth, maxWidth * aspectRatio);

							ImGui::Image(
								assignedPilots[i].IconTexture->GetDescriptorSet(),
								imageSize
							);
							std::string statusText = LOCAL(Crosswind::ToString(assignedPilots[i].PilotStatus));
							ImGui::SetCursorPosX(((maxWidth + ImGui::GetStyle().FramePadding.x * 2) - ImGui::CalcTextSize(statusText.c_str()).x) * 0.5f);
							ImGui::Text(statusText.c_str());
							ImGui::EndTooltip();
						}
					}
					//aircraft
					ImGui::TableSetColumnIndex(3);
					ImGui::Text((assignedAircraft[i].type + " (" + assignedAircraft[i].code + ")").c_str());

					ImGui::TableSetColumnIndex(4);
					if (!loadoutNames.empty())
					{
						ImGui::Combo(("Loadout##" + std::to_string(i)).c_str(), &assignedAircraft[i].loadoutIndex, loadoutNames.data(),
							static_cast<int>(loadoutNames.size()));
					}
					else
					{
						ImGui::Text("No loadouts available");
					}


					ImGui::TableSetColumnIndex(5);
					if (ImGui::Button(("Remove##" + std::to_string(i)).c_str())) {
						assignedPilots.erase(assignedPilots.begin() + i);
						assignedAircraft.erase(assignedAircraft.begin() + i);
						--i; // Re-check this row
					}
					
					ImGui::PopID();
					
				}
				ImGui::EndTable();
			}
			PilotData& selectedPlayerPilotData(m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex]);
			Squadron& playerSquadron = m_CampaignData.CurrentTheater.GetSquadronsFromID(selectedPlayerPilotData.SquadronId);

			std::sort(playerSquadron.Pilots.begin(), playerSquadron.Pilots.end(), [](const PilotData& a, const PilotData& b) {
				return a.Rank.level > b.Rank.level;
				});

			ImGui::Dummy(ImVec2(0, 50));

			//add extra pilots
			ImVec2 TableSize = ImVec2(m_WindowSize.x - m_WindowSize.x * 0.25f, 0.f);
			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			if (ImGui::BeginTable("AvailablePilots", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, TableSize))
			{
				ImGui::TableSetupColumn("Pilot Name");
				ImGui::TableSetupColumn("Assign");
				ImGui::TableHeadersRow();

				// Count aircraft that are ready and unassigned
				int availableAircraft = 0;
				for (const auto& aircraft : playerSquadron.activeAircraft)
				{
					if (aircraft.status == EAIRCRAFT_READY &&
						std::find_if(m_Mission.assignedAircraft.begin(), m_Mission.assignedAircraft.end(),
							[&](const auto& a) { return a.code == aircraft.code && a.type == aircraft.type; }) == m_Mission.assignedAircraft.end())
					{
						availableAircraft++;
					}
				}

				for (auto& pilot : playerSquadron.Pilots)
				{
					// Skip non-ready pilots
					if ((pilot.PilotStatus & EPILOT_READY) == 0)
						continue;

					// Skip pilots already assigned
					bool alreadyAssigned = std::any_of(
						m_Mission.AssignedPilots.begin(), m_Mission.AssignedPilots.end(),
						[&](const PilotData& assigned) {
							return (assigned.PilotName == pilot.PilotName && assigned.Rank.id == pilot.Rank.id);
						});
					if (alreadyAssigned)
						continue;

					
					ImGui::TableNextRow();
					
					//Name
					ImGui::TableSetColumnIndex(0);
					if (pilot.bIsPlayerPilot) ImGui::PushStyleColor(ImGuiCol_Text, PLAYERCOLOUR);
					std::string nameText = pilot.Rank.abbrevation + " " + pilot.PilotName;
					ImGui::Text(nameText.c_str());
					if (pilot.bIsPlayerPilot) ImGui::PopStyleColor();
					if (ImGui::IsItemHovered())
					{
						// Load texture if not already loaded
						if (!pilot.IconTexture && !pilot.IconPath.empty())
						{
							pilot.IconTexture = std::make_shared<Walnut::Image>(pilot.IconPath); // Replace with your actual loader if different
						}

						if (pilot.IconTexture && pilot.IconTexture->GetWidth() > 0 && pilot.IconTexture->GetHeight() > 0)
						{
							ImGui::BeginTooltip();
							float maxWidth = 120.0f; // limit tooltip width
							float aspectRatio = (float)pilot.IconTexture->GetHeight() / pilot.IconTexture->GetWidth();
							ImVec2 imageSize = ImVec2(maxWidth, maxWidth * aspectRatio);

							ImGui::Image(
								pilot.IconTexture->GetDescriptorSet(),
								imageSize
							);
							std::string statusText = LOCAL(Crosswind::ToString(pilot.PilotStatus));
							ImGui::SetCursorPosX(((maxWidth + ImGui::GetStyle().FramePadding.x * 2) - ImGui::CalcTextSize(statusText.c_str()).x) * 0.5f);
							ImGui::Text(statusText.c_str());
							ImGui::EndTooltip();
						}
					}
					//assign
					ImGui::TableSetColumnIndex(1);
					bool canAdd = m_Mission.AssignedPilots.size() < 9 && availableAircraft > 0;

					if (canAdd)
					{
						if (ImGui::Button(("Assign##" + pilot.PilotName).c_str()))
						{
							// Assign pilot
							m_Mission.AssignedPilots.push_back(pilot);

							// Assign a matching aircraft
							auto it = std::find_if(playerSquadron.activeAircraft.begin(), playerSquadron.activeAircraft.end(),
								[&](const auto& aircraft) {
									return aircraft.status == EAIRCRAFT_READY &&
										std::find_if(m_Mission.assignedAircraft.begin(), m_Mission.assignedAircraft.end(),
											[&](const auto& a) { return (a.code == aircraft.code && a.type == aircraft.type); }) == m_Mission.assignedAircraft.end();
								});
							if (it != playerSquadron.activeAircraft.end())
							{
								m_Mission.assignedAircraft.push_back(*it);
							}
						}
					}
					else
					{
						ImGui::BeginDisabled();
						ImGui::Button("Assign");
						ImGui::EndDisabled();
					}

					
				}
				ImGui::EndTable();

				ImGui::Dummy(ImVec2(0, 50));
				ImGui::SetCursorPosX((m_WindowSize.x - (ImGui::CalcTextSize(LOCAL("Button_Back")).x + ImGui::CalcTextSize(LOCAL("Button_Next")).x + (ImGui::GetStyle().FramePadding.x * 4) + ImGui::GetStyle().ItemSpacing.x)) * 0.5f);
				if (ImGui::Button(LOCAL("Button_Back")))
				{
					PreviousStage();
				}
				ImGui::SameLine();
				if (ImGui::Button(LOCAL("Button_Next")))
				{
					NextStage();
					const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
						playerSquadron.locations,
						m_CampaignData.CurrentDateTime,
						[](const ValueStartDate& e) { return e.startDate; }
					);

					Airfield SquadronAirfield = m_CampaignData.CurrentTheater.GetAirfields(m_CampaignData.CurrentDateTime)->LoadedAirfields.at(AirfieldValue->value);
					ImVec2 SquadronLocation = MapWidget.WorldPosToMapPos(SquadronAirfield.position);

					MapWidget.LerpToMapPosition(SquadronLocation, 5.0f);
				}
			}
			break;
		}
				
		case 2: //flight plan
		{
			ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
			ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("MissionFlightPlan")).x) * 0.5f);
			ImGui::Text(LOCAL("MissionFlightPlan"));
			ImGui::PopFont();
			ImGui::Separator();

			MapWidget.ClearItems();
			if (true)
			{
				int count = 0;
				for (auto& wp : m_Mission.missionPlan.missionWaypoints) {
					ImVec2 mapPos = MapWidget.WorldPosToMapPos(wp.WorldLocation);

					MapItem item;
					item.id = "waypoint" + std::to_string(count);
					item.mapPosition = mapPos;
					item.widgetSize = ImVec2(16, 16); // Small square size
					std::string label = wp.name;
					item.renderWidget = [&, count, label](ImVec2 screenPos, ImVec2 size) {
						ImVec2 topLeft = screenPos;
						ImVec2 bottomRight = ImVec2(screenPos.x + size.x, screenPos.y + size.y);
						auto* drawList = ImGui::GetWindowDrawList();
						
						// Draw square
						if (wp.canBeEdited) 
						{
							drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 200, 0, 255), 3.0f); // Yellow
						}
						else
						{
							drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(150, 100, 0, 255), 3.0f);
						}
						drawList->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, 255));



						// Draw label
						ImVec2 textPos = ImVec2(bottomRight.x + 4.0f, topLeft.y);
						drawList->AddText(textPos, IM_COL32(0, 0, 0, 255), label.c_str());




						// Make it draggable
						
							ImGui::SetCursorScreenPos(topLeft);
							ImGui::InvisibleButton(("##drag" + std::to_string(count)).c_str(), size);
						if (wp.canBeEdited) {
							if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
								Vec3 worldPos = MapWidget.GetCursorWorldPosition();
								// Update waypoint
								wp.WorldLocation.x = worldPos.x;
								wp.WorldLocation.z = worldPos.z;
							}
						}

						// Tooltip
						if (ImGui::IsItemHovered() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
							std::string tooltip = "Speed: " + std::to_string(wp.speed) + " km/h\nAltitude: " + Core::FloatToString(wp.WorldLocation.y, 0) + " m";
							ImGui::SetTooltip("%s", tooltip.c_str());
							if (wp.canBeEdited)
							{
								// Open popup on right-click
								if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
									ImGui::OpenPopup(("WaypointPopup##" + std::to_string(count)).c_str());
								}
	
							}
						}

						// Context menu on right-click
						if (ImGui::BeginPopupContextItem(("WaypointPopup##" + std::to_string(count)).c_str())) {
							ImGui::Text("Edit Waypoint");
							ImGui::InputInt("Speed (km/h)", &wp.speed);
							ImGui::InputFloat("Altitude (m)", &wp.WorldLocation.y, 10.0f, 100.0f, "%.1f");
							ImGui::EndPopup();
						}
					};
					count++;
					MapWidget.AddItem(item);
				}
				for (size_t i = 1; i < m_Mission.missionPlan.missionWaypoints.size(); ++i) {
					Vec3 worldA = m_Mission.missionPlan.missionWaypoints[i - 1].WorldLocation;
					Vec3 worldB = m_Mission.missionPlan.missionWaypoints[i].WorldLocation;

					ImVec2 mapA = MapWidget.WorldPosToMapPos(worldA);
					ImVec2 mapB = MapWidget.WorldPosToMapPos(worldB);
					ImVec2 offset = ImVec2(mapB.x - mapA.x, mapB.y - mapA.y);

					MapItem line;
					line.mapPosition = mapA;
					line.widgetSize = { 1, 1 };      // Ignored
					line.minSize = 0.0f;
					line.maxSize = 3.0f;             // Can be small — ignored for drawing

					line.renderWidget = [offset](ImVec2 screenPos, ImVec2 size) {
						ImDrawList* drawList = ImGui::GetWindowDrawList();
						drawList->AddLine(screenPos, ImVec2(screenPos.x + offset.x * size.x, screenPos.y + offset.y * size.y), IM_COL32(50, 50, 50, 125), 4.0f * size.x);
						};

					MapWidget.AddItem(line);
				}

				hasAddedWaypoints = true;
			}

			//Set map frontlines
			if (m_CampaignData.CurrentTheater.LoadedFrontlines.size() == 0)
			{
				m_CampaignData.CurrentTheater.LoadFrontLines(m_CampaignData.CurrentDateTime);
			}
			if (!MapWidget.HasFrontlines())
			{
				MapWidget.UpdateFrontlines(m_CampaignData.CurrentTheater.LoadedFrontlines);
			}

			MapWidget.Draw("CampaignMap", ImVec2(ImGui::GetContentRegionAvail().x - 350, ImGui::GetContentRegionAvail().y));
			ImGui::SameLine();
			ImGui::BeginChild("##FlightPlanMap", ImVec2(350, ImGui::GetContentRegionAvail().y), false);

			ImGui::SetCursorPosX(ImGui::GetStyle().FramePadding.x);
			ImGui::TextWrapped(LOCAL("FlightPlanGuide"));
			ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
			ImGui::SetCursorPosX(ImGui::GetStyle().FramePadding.x);
			ImGui::TextWrapped(LOCAL("FlightPlanGuide2"));

			ImGui::Dummy(ImVec2(0, 50));

			ImGui::SetCursorPosX((350 - ImGui::CalcTextSize(LOCAL("Button_GenerateMission")).x - ImGui::GetStyle().FramePadding.x * 2) * 0.5f);
			if (ImGui::Button(LOCAL("Button_GenerateMission")))
			{
				CurrentAction = EWindowActions::NEXT;
			}
			ImGui::SetCursorPosX((350 - ImGui::CalcTextSize(LOCAL("Button_Back")).x - ImGui::GetStyle().FramePadding.x * 2) * 0.5f);
			if (ImGui::Button(LOCAL("Button_Back")))
			{
				PreviousStage();
			}

			ImGui::EndChild();
		}
			break;
		}

		ImGui::EndPopup();
	}
}

void MissionBriefingWindow::OnUpdate(float deltaTime)
{
	//Set map texture
	bool fileExists = std::filesystem::exists(m_CampaignData.CurrentTheater.MapTexturePath);
	if (!fileExists)
	{
		std::cerr << "Map texture not found." << std::endl;
	}
	if (!MapWidget.bMapTextureSet) //dont want to be setting the texture on every frame
	{
		MapWidget.SetMapTexture(m_CampaignData.CurrentTheater.MapTexturePath, m_CampaignData.CurrentTheater.MapTextureSize, m_CampaignData.CurrentTheater.MapWorldSize);
	}


}

void MissionBriefingWindow::NextStage()
{
	if (briefingStage != 2)
	{
		briefingStage++;
	}
}

void MissionBriefingWindow::PreviousStage()
{
	if (briefingStage != 0)
	{
		briefingStage--;
	}
}

void MissionBriefingWindow::DisplayWindProfile(const WindProfile& wind)
{
	static const std::vector<int> altitudes = { 500, 1000, 2000, 5000 };

	ImGui::Text(LOCAL("WeatherWind"));
	for (size_t i = 0; i < wind.windLevels.size() && i < altitudes.size(); ++i) {
		float direction = wind.windLevels[i].first;
		float speed = wind.windLevels[i].second;
		std::string bulletText = std::to_string(altitudes[i]) + LOCAL("MeterAbbr") + ": " + Core::FloatToString(speed,1) + LOCAL("MeterPerSecAbbr") + " " + LOCAL("MeterPerSecAbbr") + " " + LOCAL("DirectionFrom")+ " " + Core::FloatToString(direction, 1);
		ImGui::BulletText(bulletText.c_str());
	}
}
