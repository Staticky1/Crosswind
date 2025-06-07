#include "MainMenuButtons.h"
#include "Walnut/Application.h"

MainMenuButtons::MainMenuButtons()
	: CrosswindWindow(LOCAL("Title"))
{
	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_hasCloseButton = false;
}

void MainMenuButtons::RenderContents()
{
	if (!bShowButtons) ImGui::BeginDisabled();
	m_WindowSize = ImVec2(ImGui::CalcTextSize(LOCAL("Title")).x + 50, 300);
	//Get center of screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));

	ImVec2 ButtonSize = ImVec2(ImGui::GetContentRegionAvail().x * 0.8f, ImGui::GetFontSize() * 2);
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - (ButtonSize.x / 2));
	if (ImGui::Button(LOCAL("NewCampaign"), ButtonSize))
	{
		bNewCampaignPressed = true;
	}
	//ImGui::AlignTextToFramePadding();
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - (ButtonSize.x / 2));
	if (ImGui::Button(LOCAL("LoadCampaign"), ButtonSize))
	{
		bLoadCampaignPressed = true;
	}

	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - (ButtonSize.x / 2));
	if (ImGui::Button(LOCAL("Settings"), ButtonSize))
	{
		bSettingsPressed = true;
	}

	ImGui::Dummy(ImVec2(0.0f, ButtonSize.y * 1.5f));
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - (ButtonSize.x / 2));
	if (ImGui::Button(LOCAL("Exit"), ButtonSize))
	{
		bExitPressed = true;
	}
	if (!bShowButtons) ImGui::EndDisabled();
}

void MainMenuButtons::OnUpdate(float deltaTime)
{
	//Update size and location of window on each frame

	if (bExitPressed)
	{
		Walnut::Application::Get().Close();
		bExitPressed = false;
	}

	if (bNewCampaignPressed)
	{
		
	}

	if (bLoadCampaignPressed)
	{
		
	}

	if (bSettingsPressed)
	{
		
	}
}
