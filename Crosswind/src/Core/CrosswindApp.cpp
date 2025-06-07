#include "CrosswindApp.h"
#include "UI/MainMenuUI.h"
#include "UI/CampaignLayer.h"

CrosswindApp::CrosswindApp(Walnut::ApplicationSpecification spec)
	:Walnut::Application(spec)
{

}

void CrosswindApp::StartApp()
{
	//create mainmenu layer
	MainMenu = std::make_shared<MainMenuLayer>();
	PushLayer(MainMenu);
}

void CrosswindApp::SetCampaignData(const CampaignData& inCampaignData)
{
	loadedCampaignData = inCampaignData;
}

CampaignData& CrosswindApp::GetCampaignData()
{
	return loadedCampaignData;
}

void CrosswindApp::PushCampaignScreen()
{
	if (loadedCampaignData.CampaignName != "")
	{
		if (MainMenu)
		{
			RemoveLayer(MainMenu);
		}

		m_CampaignLayer = std::make_shared<CampaignLayer>();
		PushLayer(m_CampaignLayer);
	}
}
