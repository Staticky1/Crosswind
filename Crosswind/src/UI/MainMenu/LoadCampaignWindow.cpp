#include "LoadCampaignWindow.h"
#include <filesystem>
#include "Campaign/CampaignManager.h"

LoadCampaignWindow::LoadCampaignWindow()
	:CrosswindWindow(LOCAL("LoadCampaign"))
{
	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_hasCloseButton = false;
}

void LoadCampaignWindow::RenderContents()
{
	m_WindowSize = ImVec2(ImGui::CalcTextSize(LOCAL("LoadCampaign")).x + 300, 500);
	//Get center of screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));


	// Render the listbox
	if (ImGui::BeginListBox("##CampaignList", ImVec2(-FLT_MIN, 350)))
	{
		for (int i = 0; i < m_campaignNames.size(); ++i)
		{
			const bool isSelected = (m_SaveItemSelectedIndex == i);
			if (ImGui::Selectable(m_campaignNames[i].c_str(), isSelected))
				m_SaveItemSelectedIndex = i;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	// Buttons
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();



	if (m_SaveItemSelectedIndex < 0) ImGui::BeginDisabled();
	if (ImGui::Button(LOCAL("Button_Load")) && m_SaveItemSelectedIndex >= 0)
	{
		m_LoadButtonPressed = true;
	}
	if (m_SaveItemSelectedIndex < 0) ImGui::EndDisabled();

	ImGui::SameLine();
	ImGui::SetCursorPosX(m_WindowSize.x - ImGui::CalcTextSize(LOCAL("Button_Cancel")).x - 20);
	if (ImGui::Button(LOCAL("Button_Cancel")))
	{
		m_CancelButtonPressed = true;
	}

}

void LoadCampaignWindow::OnUpdate(float deltaTime)
{
	if (m_campaignNames.empty())
	{
		for (const auto& entry : std::filesystem::directory_iterator("data/saves/"))
		{
			if (!entry.is_regular_file()) continue;

			const std::string filename = entry.path().filename().string();

			// Must end with ".xml" and not contain "_backup" before the extension
			if (entry.path().extension() == ".xml" && filename.find("_backup") == std::string::npos)
			{
				// Strip ".xml" to get campaign name
				std::string campaignName = entry.path().stem().string();  // e.g., "campaignname.xml" "campaignname"
				m_campaignNames.push_back(campaignName);
			}
		}
	}

	if (m_LoadButtonPressed)
	{
		CampaignManager::Instance().LoadCampaignSaveFile(m_campaignNames[m_SaveItemSelectedIndex]);
		m_LoadButtonPressed = false;
		m_SaveItemSelectedIndex = -1;
		CurrentAction = EWindowActions::NEXT;
	}

	if (m_CancelButtonPressed)
	{
		CurrentAction = EWindowActions::CLOSE;
		m_CancelButtonPressed = false;
	}
}
