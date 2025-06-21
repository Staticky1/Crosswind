#include "SquadronInfoWindow.h"
#include "Core/CrosswindApp.h"
#include "Core/EnumHelpers.h"
#include "Walnut/Image.h"
#include "Campaign/CampaignManager.h"

SquadronInfoWindow::SquadronInfoWindow(std::shared_ptr<CampaignContext> context)
	:CrosswindWindow("##SquadronInfo")
	,m_Context(context)
	,m_CampaignData(context->campaignData)
	,m_SelectedPlayerPilotIndex(context->selectedPlayerPilotIndex)
{
	
	m_WindowFlags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar;
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 24, ImGui::GetMainViewport()->WorkSize.y - 120);
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));
}

void SquadronInfoWindow::RenderContents()
{
	//recalculate window pos and size
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 24, ImGui::GetMainViewport()->WorkSize.y - 120);
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2) + 58));

	PilotData& selectedPlayerPilotData(m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex]);
	Squadron& playerSquadron = m_CampaignData.CurrentTheater.GetSquadronsFromID(selectedPlayerPilotData.SquadronId);
	std::string squadronTitle = playerSquadron.name + " (" + playerSquadron.id + ")";

	//heading
	if (!m_SquadronLogo)
	{
		m_SquadronLogo = std::make_shared<Walnut::Image>(playerSquadron.iconPath);
	}
	ImVec2 ImageSize = ImVec2(64, 64);

	ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
	ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(squadronTitle.c_str()).x - ImageSize.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f);
	ImGui::Image(m_SquadronLogo->GetDescriptorSet(), ImageSize); ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::Text(squadronTitle.c_str());
	ImGui::PopFont();

	ImGui::Dummy(ImVec2(0, 40));

	//Active Roster
	ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("SquadronWindow_ActiveRoster")).x) * 0.5f);
	ImGui::Text(LOCAL("SquadronWindow_ActiveRoster"));

	DrawPilotTable(playerSquadron.Pilots, EPILOT_READY, "##ActiveTable");

	ImGui::Dummy(ImVec2(0, 40));
	//Medical Bay
	ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("SquadronWindow_MedicalBay")).x) * 0.5f);
	ImGui::Text(LOCAL("SquadronWindow_MedicalBay"));

	DrawPilotTable(playerSquadron.Pilots, EPILOT_WOUNDED,"##WoundedTable", true);

	//lost pilots
	ImVec2 TableSize = ImVec2(m_WindowSize.x - m_WindowSize.x * 0.3f, 50.f);
	ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);

	if (ImGui::TreeNode(LOCAL("SquadronWindow_LostPilots")))
	{
		DrawPilotTable(playerSquadron.Pilots, EPILOT_DEAD | EPILOT_CAPTURED, "##LostPilotTable");
		ImGui::TreePop();
	}

	ImGui::Dummy(ImVec2(0, 40));
	//aircraft
	ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("SquadronWindow_ActiveAircraft")).x) * 0.5f);
	ImGui::Text(LOCAL("SquadronWindow_ActiveAircraft"));

	DrawAircraftTable(playerSquadron.activeAircraft, EAIRCRAFT_READY, "##AircraftTable");
	//maintance
	ImGui::Dummy(ImVec2(0, 40));
	ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("SquadronWindow_GroundedAircraft")).x) * 0.5f);
	ImGui::Text(LOCAL("SquadronWindow_GroundedAircraft"));
	DrawAircraftTable(playerSquadron.activeAircraft, EAIRCRAFT_MAINTANCE, "##MaintanceTable", true);
}

void SquadronInfoWindow::OnUpdate(float deltaTime)
{
	PilotData& selectedPlayerPilotData = m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex];
}

void SquadronInfoWindow::DrawPilotTable(std::vector<PilotData>& pilots, EPilotStatus statusFilter, std::string tableId, bool bShowWoundedDaysColumn)
{
	// Filter only ready pilots
	std::vector<PilotData*> readyPilots;
	for (auto& pilot : pilots)
	{
		if (!HasStatus(statusFilter, pilot.PilotStatus))
			continue;
			readyPilots.push_back(&pilot);
	}

	// Sort by rank level descending
	std::sort(readyPilots.begin(), readyPilots.end(), [](const PilotData* a, const PilotData* b) {
		return a->Rank.level > b->Rank.level;
		});

	// Table headers
	std::vector<const char*> headers = {
		"Name", "Missions", "Aircraft", "Vehicle", "Rail", "Weapon", "Building", "Marine"
	};

	if (bShowWoundedDaysColumn)
	{
		headers.push_back("Days Until Recovered");
	}
	int tableColumns = headers.size();

	ImVec2 TableSize = ImVec2(m_WindowSize.x - m_WindowSize.x * 0.3f, 0.f);
	ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
	if (ImGui::BeginTable(tableId.c_str(), tableColumns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit, TableSize))
	{
		// Set the name column to fit the content
		ImGui::TableSetupColumn(LOCAL("Name"), ImGuiTableColumnFlags_WidthStretch, 0.0f); // Stretch to fill long names

		if (bShowWoundedDaysColumn)
		{
			ImGui::TableSetupColumn(LOCAL("DaysUntilRecovered"), ImGuiTableColumnFlags_WidthFixed, 200.0f);
		}
		// Fixed small width for number columns
		ImGui::TableSetupColumn(LOCAL("Missions"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn(LOCAL("Aircraft"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn(LOCAL("Vehicles"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn(LOCAL("Rail"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn(LOCAL("Weapons"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn(LOCAL("Buildings"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
		ImGui::TableSetupColumn(LOCAL("Marine"), ImGuiTableColumnFlags_WidthFixed, 70.0f);

		ImGui::TableHeadersRow();

		ImVec4 PlayerRowColour = PLAYERCOLOUR;

		for (PilotData* pilot : readyPilots)
		{
			if (pilot->bIsPlayerPilot) ImGui::PushStyleColor(ImGuiCol_Text, PlayerRowColour);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", std::string(pilot->Rank.abbrevation + " " + pilot->PilotName).c_str());

			if (ImGui::IsItemHovered())
			{
				// Load texture if not already loaded
				if (!pilot->IconTexture && !pilot->IconPath.empty())
				{
					pilot->IconTexture = std::make_shared<Walnut::Image>(pilot->IconPath); // Replace with your actual loader if different
				}

				if (pilot->IconTexture && pilot->IconTexture->GetWidth() > 0 && pilot->IconTexture->GetHeight() > 0)
				{
					ImGui::BeginTooltip();
					float maxWidth = 120.0f; // limit tooltip width
					float aspectRatio = (float)pilot->IconTexture->GetHeight() / pilot->IconTexture->GetWidth();
					ImVec2 imageSize = ImVec2(maxWidth, maxWidth * aspectRatio);

					ImGui::Image(
						pilot->IconTexture->GetDescriptorSet(),
						imageSize
					);
					std::string statusText = LOCAL(Crosswind::ToString(pilot->PilotStatus));
					ImGui::SetCursorPosX(((maxWidth + ImGui::GetStyle().FramePadding.x * 2) - ImGui::CalcTextSize(statusText.c_str()).x) * 0.5f);
					ImGui::Text(statusText.c_str());
					ImGui::EndTooltip();
				}
			}

			if (bShowWoundedDaysColumn)
			{
				ImGui::TableNextColumn(); ImGui::Text("%d", pilot->woundDuration);
			}

			ImGui::TableNextColumn(); ImGui::Text("%d", pilot->Missions);

			if (pilot->bIsPlayerPilot) ImGui::PopStyleColor();
			std::vector<std::string> victoryCategories = {"AIRCRAFT", "VEHICLE", "RAIL", "WEAPON", "BUILDING", "MARINE"};


			for (const auto& category : victoryCategories) // Example: AIRCRAFT, VEHICLE, etc.
			{
				if (pilot->bIsPlayerPilot) ImGui::PushStyleColor(ImGuiCol_Text, PlayerRowColour);
				int categoryCount = Crosswind::CountVictoriesByCategory(*pilot, category);
				std::unordered_map<EVictoryType, int> breakdown = Crosswind::GetVictoryBreakdown(*pilot, category);

				ImGui::TableNextColumn();
				ImGui::Text("%d", categoryCount);
				if (pilot->bIsPlayerPilot) ImGui::PopStyleColor();
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					std::string text = LOCAL(Crosswind::GetCategoryName(category)) + std::string(" ") + LOCAL("Victories");
					ImGui::Text(text.c_str());
					ImGui::Separator();

					for (const auto& [type, count] : breakdown)
					{
						ImGui::Text("%s: %d", Crosswind::ToString<EVictoryType>(type), count);
					}

					ImGui::EndTooltip();
				}
			}
		}

		ImGui::EndTable();
	}
}

void SquadronInfoWindow::DrawAircraftTable(const std::vector<SquadronAircraft>& aircraftList, ESquadronAircraftStatus filterStatus, std::string tableId, bool showMaintDuration)
{
	int columnCount = 2; // Type + Code
	if (showMaintDuration)
		columnCount++;

	if (CampaignManager::Instance().GetAircraftList().size() == 0)
	{
		CampaignManager::Instance().LoadAircraftDataFromXML("data/aircraft/Aircraft.xml");
	}
	ImVec2 TableSize = ImVec2(m_WindowSize.x - m_WindowSize.x * 0.3f, 0.f);
	ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
	if (ImGui::BeginTable(tableId.c_str(), columnCount, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, TableSize))
	{
		ImGui::TableSetupColumn(LOCAL("AircraftType"));
		ImGui::TableSetupColumn(LOCAL("AircraftCode"));

		if (showMaintDuration)
			ImGui::TableSetupColumn(LOCAL("AircraftMaintance"));

		ImGui::TableHeadersRow();

		for (const auto& aircraft : aircraftList)
		{
			if (aircraft.status != filterStatus)
				continue;

			ImGui::TableNextRow();

			// Type
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(CampaignManager::Instance().GetAircraftByType(aircraft.type.c_str())->displayName.c_str());

			// Code
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(aircraft.code.c_str());

			// Maintenance Duration (optional)
			if (showMaintDuration)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%d", aircraft.maintanceDuration);
			}
		}

		ImGui::EndTable();
	}
}