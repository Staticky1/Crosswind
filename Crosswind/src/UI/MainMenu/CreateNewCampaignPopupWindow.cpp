#include "CreateNewCampaignPopupWindow.h"
#include "Campaign/CampaignManager.h"
#include <algorithm>
#include <filesystem>
#include "Core/CrosswindApp.h"
#include "Core/CampaignData.h"

CreateNewCampaignPopupWindow::CreateNewCampaignPopupWindow(std::shared_ptr<WindowContext> context)
	:CrosswindWindow("##Loading")
	, m_Context(std::static_pointer_cast<NewPilotContext>(context))
	, m_CampaignData(m_Context->campaignData)
	, m_PilotData(m_Context->pilotData)
{
	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_WindowSize = ImVec2(std::min(ImGui::GetMainViewport()->WorkSize.x, DesiredWindowSize.x), std::min(ImGui::GetMainViewport()->WorkSize.y, DesiredWindowSize.y));
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));

}

void CreateNewCampaignPopupWindow::RenderContents()
{

		ImGui::Text("Loading...");
	
}

void CreateNewCampaignPopupWindow::OnUpdate(float deltaTime)
{
	if (bIsLoading)
	{
		if (!bHasCreatedSaveData)
		{
			if (CampaignManager::Instance().CreateNewCampaignSave(m_CampaignData, m_PilotData))
			{
				bHasCreatedSaveData = true;
			}
			else
			{
				bIsLoading = false;
				std::cerr << "Failed to create Save Data!" << std::endl;
			}
		}
		else
		{
			if (!bHasLoadedData)
			{
				std::string filepath = "data/saves/" + m_CampaignData.NewCampaignName + ".xml";

				if (std::filesystem::exists(filepath))
				{
					if (CampaignManager::Instance().LoadCampaignSaveFile(m_CampaignData.NewCampaignName))
					{
						bHasLoadedData = true;
						CurrentAction = EWindowActions::NEXT;
					}
				}
			}
			else
			{
				bIsLoading = false;
			}
		
		}


	}
	else
	{
		CurrentAction = EWindowActions::NEXT;
	}
}

std::unique_ptr<CrosswindWindow> CreateNewCampaignPopupWindow::CreateNextWindow(std::shared_ptr<WindowContext> context)
{
	return std::unique_ptr<CrosswindWindow>();
}

std::unique_ptr<CrosswindWindow> CreateNewCampaignPopupWindow::CreateBackWindow(std::shared_ptr<WindowContext> context)
{
	return std::unique_ptr<CrosswindWindow>();
}
