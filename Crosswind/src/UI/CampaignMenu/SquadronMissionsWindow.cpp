#include "SquadronMissionsWindow.h"
#include "Core.h"
#include "Core/CrosswindApp.h"
#include "Campaign/CampaignManager.h"
#include "UI/CampaignLayer.h"

SquadronMissionsWindow::SquadronMissionsWindow(std::shared_ptr<CampaignContext> context)
	:CrosswindWindow("##SquadronMission")
	, m_Context(context)
	, m_CampaignData(context->campaignData)
	, m_SelectedPlayerPilotIndex(context->selectedPlayerPilotIndex)
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

void SquadronMissionsWindow::RenderContents()
{
	//recalculate window pos and size
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 24, ImGui::GetMainViewport()->WorkSize.y - 120);
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2) + 58));

	PilotData& selectedPlayerPilotData(m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex]);
	Squadron& playerSquadron = m_CampaignData.CurrentTheater.GetSquadronsFromID(selectedPlayerPilotData.SquadronId);
	std::string squadronTitle = playerSquadron.name + " (" + playerSquadron.id + ")";

	ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
	ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("Scheduled_Missions")).x) * 0.5f);
	ImGui::AlignTextToFramePadding();
	ImGui::Text(LOCAL("Scheduled_Missions"));
	ImGui::PopFont();

	if (playerSquadron.currentDaysMissions.size() > 0)
	{
		for (auto& mission : playerSquadron.currentDaysMissions)
		{		



			ImVec2 TableSize = ImVec2(m_WindowSize.x - m_WindowSize.x * 0.5f, 0.f);
			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			ImGui::Text(mission.missionPlan.missionType.name.c_str());
			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			ImGui::Text(LOCAL("MissionDescription"));
			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			ImGui::TextWrapped(mission.missionPlan.missionType.description.c_str());

			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			ImGui::Text(LOCAL("MissionDateTime")); ImGui::SameLine(); ImGui::Text(mission.missionPlan.missionDate.ToFullString().c_str());

			ImGui::SetCursorPosX((m_WindowSize.x - TableSize.x) * 0.5f);
			if (ImGui::BeginTable(("##AssignedPilotsTable" + (mission.missionPlan.missionDate.ToTimeString())).c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, TableSize))
			{
				ImGui::TableSetupColumn(LOCAL("FlightPos"));
				ImGui::TableSetupColumn(LOCAL("Name"));
				ImGui::TableHeadersRow();

				int index = 0;
				ImVec4 PlayerRowColour = ImVec4(1.0f, 0.85f, 0.0f, 1.0f);

				for (const auto& pilot : mission.AssignedPilots)
				{
					if (pilot.bIsPlayerPilot) ImGui::PushStyleColor(ImGuiCol_Text, PlayerRowColour);
					std::string flightPosText;
					if (index == 0)
					{
						flightPosText = LOCAL("FlightLead");
					}
					else
					{
						std::unordered_map<std::string, std::string> nameMap = {
							{"pos", std::to_string(index)}
						};
						flightPosText = LOCAL_FORMAT("Wingman", false, nameMap);
					}
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(flightPosText.c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted((pilot.Rank.abbrevation + " " + pilot.PilotName).c_str());

					index++;

					if (pilot.bIsPlayerPilot) ImGui::PopStyleColor();
				}

				ImGui::EndTable();

				ImGui::Dummy(ImVec2(0,50));
			}

		}
	}
	else
	{
		ImGui::Dummy(ImVec2(0, 200));
		ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(LOCAL("NoMissions")).x) * 0.5f);
		ImGui::Text(LOCAL("NoMissions"));
	}

	ImGui::Dummy(ImVec2(0, 100));

	ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
	ImGui::SetCursorPosX((m_WindowSize.x - (ImGui::CalcTextSize(LOCAL("NextDay")).x + ImGui::CalcTextSize(LOCAL("CombatReport")).x + ImGui::GetStyle().ItemSpacing.x)) * 0.5f);
	if (m_CampaignData.ExpectedCombatReport == "") { ImGui::BeginDisabled(); }
	ImGui::Button(LOCAL("CombatReport"));
	if (m_CampaignData.ExpectedCombatReport == "") { ImGui::EndDisabled(); }
	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	if (playerSquadron.currentDaysMissions.size() == 0 || AllMissionComplete(playerSquadron))
	{
		if (ImGui::Button(LOCAL("NextDay")))
		{
			bRequestNewMission = true;
		};
	}
	else
	{
		if (ImGui::Button(LOCAL("StartMission")))
		{
			bStartMission = true;
		};
	}

	ImGui::PopFont();
}

void SquadronMissionsWindow::OnUpdate(float deltaTime)
{
	if (bRequestNewMission)
	{
		CampaignManager::Instance().RequestNewDay(m_CampaignData.CurrentDateTime,&m_CampaignData.CurrentTheater,m_CampaignData.PlayerPilots);
		bRequestNewMission = false;
		CampaignManager::Instance().SaveCampaignData(m_CampaignData, m_SelectedPlayerPilotIndex);
	}

	if (bStartMission)
	{
		bStartMission = false;
		PilotData& selectedPlayerPilotData(m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex]);
		Squadron& playerSquadron = m_CampaignData.CurrentTheater.GetSquadronsFromID(selectedPlayerPilotData.SquadronId);
		if (MissionContainsPlayer(*GetCurrentMission(playerSquadron)))
		{
			//Mission config screen
			CurrentAction = EWindowActions::NEXT;
		}
		else
		{
			//simulate mission
			
			if ((GetCurrentMission(playerSquadron)))
			{
				DateTime missionTime = GetCurrentMission(playerSquadron)->missionPlan.missionDate;
				SquadronMission missionPlan = *GetCurrentMission(playerSquadron);
				std::vector<EncounterResult> results;
				CampaignManager::Instance().SimulateMission(missionTime, &m_CampaignData.CurrentTheater, missionPlan, playerSquadron, results);
				CampaignManager::Instance().UpdateCampaignData(m_CampaignData, results);
				MarkMissionComplete(playerSquadron);
				CampaignManager::Instance().UpdateMissionData(playerSquadron);

				NewsItem newItem;
				newItem.newsHeading = LOCAL("MissionReport");
				std::vector<std::string> items = BuildMissionResultNews(results,true);
				

				if (results.size() > 0)
				{
					for (const auto& i : items)
					{
						newItem.newsText += i + "\n \n";
					}
				}
				else
				{
					newItem.newsText = "Nothing of note";
				}
				newItem.newsImagePath = "data/images/news/Barbarossa11.jpg";

				std::vector<std::string> otherItems = BuildMissionResultNews(results, false, true);
				std::string otherString;
				for (const auto i : otherItems)
				{
					otherString += i + "\n \n";
				}

				if (otherString != "")
				{
					newItem.customRender = [otherString]() 
					{
						if (ImGui::TreeNode(LOCAL("OtherSquadrons")))
						{
							ImGui::TextWrapped(otherString.c_str());
							ImGui::TreePop();
						}
					};
				}
				CampaignManager::Instance().GetCampaignLayer()->ClearNewsItems();
				CampaignManager::Instance().GetCampaignLayer()->AddNewsItem(newItem);
				CampaignManager::Instance().GetCampaignLayer()->ShowNewsItems();

				//set time to start of next mission if one exsits
				if (GetCurrentMission(playerSquadron))
				{
					m_CampaignData.CurrentDateTime = GetCurrentMission(playerSquadron)->missionPlan.missionDate;
				}
				else
				{
					m_CampaignData.CurrentDateTime.AddMinutes(60); //if no mission just add on hour
				}
				
			}

		}

		
	}
}

bool SquadronMissionsWindow::AllMissionComplete(const Squadron& playerSquadron)
{
	for (const auto& m : playerSquadron.currentDaysMissions)
	{
		if (!m.missionComplete)
		{
			return false;
			break;
		}
	}

	return true;
}

const SquadronMission* SquadronMissionsWindow::GetCurrentMission(const Squadron& playerSquadron)
{
	for (const auto& m : playerSquadron.currentDaysMissions)
	{
		if (!m.missionComplete)
		{
			return &m;
			break;
		}
	}
	return nullptr;
}

void SquadronMissionsWindow::MarkMissionComplete(Squadron& playerSquadron)
{
	for (int i = 0; i < playerSquadron.currentDaysMissions.size(); ++i)
	{
		if (!playerSquadron.currentDaysMissions[i].missionComplete)
		{
			playerSquadron.currentDaysMissions[i].missionComplete = true;
			return;
		}
	}
}

bool SquadronMissionsWindow::MissionContainsPlayer(const SquadronMission& mission)
{
	for (const auto& pilot : mission.AssignedPilots)
	{
		if (pilot.bIsPlayerPilot)
		{
			return true;
			break;
		}
	}
	
	return false;
}

std::vector<std::string> SquadronMissionsWindow::BuildMissionResultNews(std::vector<EncounterResult>& results, bool excludeOtherSquads, bool excludeOwnSquad)
{
	std::vector<std::string> returnStrings;

	//Get results for squadron
	for (const auto& r : results)
	{
		std::string squadAId = r.attackingPilot.first.SquadronId;
		std::string squadBId = r.targetPilot.first.SquadronId;
		std::string playerSquadId = m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex].SquadronId;

		if (((playerSquadId == squadAId || playerSquadId == squadBId) && excludeOwnSquad) ||
		 ((playerSquadId != squadAId && playerSquadId != squadBId) && excludeOtherSquads)) continue;

		std::vector<std::string> newsItem;
		//victories
		std::string resultType;

		std::string attackerName = r.attackingPilot.first.Rank.abbrevation + " " + r.attackingPilot.first.PilotName + "(" + r.attackingPilot.second.type + ")";
		std::string targetName = r.targetPilot.first.Rank.abbrevation + " " + r.targetPilot.first.PilotName + "(" + r.targetPilot.second.type + ")";
		newsItem.push_back(squadAId);
		newsItem.push_back(attackerName);
		
		std::string resultSuffix;
		std::string locationSuffix;
		if (r.result == EEncounterResult::SHOTDOWN_BAILED || r.result == EEncounterResult::SHOTDOWN_KILLED || r.result == EEncounterResult::SHOTDOWN_CRASHLANDING)
		{
			resultType = "shot down";
		}
		else if (r.result == EEncounterResult::DAMAGED_MEDIUM || r.result == EEncounterResult::DAMAGED_MINOR)
		{
			resultType = "damaged";
		}
		else
		{
			resultType = "severely damaged";
		}

		newsItem.push_back(resultType);
		newsItem.push_back(squadBId);
		newsItem.push_back(targetName);
		newsItem.push_back(".");
		
		switch (r.result)
		{
		case EEncounterResult::SHOTDOWN_KILLED:
			resultSuffix = "was killed over";
			break;
		case EEncounterResult::SHOTDOWN_BAILED:
			resultSuffix = "bailed out over";
			break;
		case EEncounterResult::SHOTDOWN_CRASHLANDING:
			resultSuffix = "crash landed in";
			break;
		default:
			resultSuffix = "";
			break;
		}
				

		if (r.result == EEncounterResult::SHOTDOWN_BAILED || r.result == EEncounterResult::SHOTDOWN_KILLED || r.result == EEncounterResult::SHOTDOWN_CRASHLANDING)
		{
			newsItem.push_back(targetName);
			newsItem.push_back(resultSuffix);
			std::pair<std::string, float> location = Theater::GetPointSideFromFrontlines(r.worldLocation, m_CampaignData.CurrentTheater.GetFrontlines(m_CampaignData.CurrentDateTime));
				
			if (location.first == "Allies")
			{
				locationSuffix = "Allied territory";
			}
			else
			{
				locationSuffix = "Axis territory";
			}

			newsItem.push_back(locationSuffix);
			newsItem.push_back(".");

			
		}

		returnStrings.push_back(Core::CompileNewsStrings(newsItem));

	}

	return returnStrings;

}
