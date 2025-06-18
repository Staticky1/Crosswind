#include "MissionBriefingWindow.h"
#include "Core/CrosswindApp.h"

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


	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 50, ImGui::GetMainViewport()->WorkSize.y - 240);

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
			ImGui::Text(std::to_string(m_CampaignData.CurrentWeather.airTemp).c_str());

			ImGui::Text(LOCAL("WeatherAirPressure")); ImGui::SameLine();
			ImGui::Text(std::to_string(m_CampaignData.CurrentWeather.airPressure).c_str());


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

			break;
		}
		case 1: //pilots and loadout

			break;
		case 2: //flight plan

			break;
		}

		ImGui::EndPopup();
	}
}

void MissionBriefingWindow::OnUpdate(float deltaTime)
{
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
