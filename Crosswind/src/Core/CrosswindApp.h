#pragma once

#include "Walnut/Application.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

#include "CampaignData.h"

class MainMenuLayer;
class CampaignLayer;

class CrosswindApp : public Walnut::Application
{
public:
	CrosswindApp(Walnut::ApplicationSpecification spec);

	void StartApp();

	void SetCampaignData(const CampaignData& inCampaignData);
	CampaignData& GetCampaignData();

	void PushCampaignScreen();

private:

	bool bCampaignLoaded = false;
	CampaignData loadedCampaignData;

	std::shared_ptr<MainMenuLayer> MainMenu; 
	std::shared_ptr<CampaignLayer> m_CampaignLayer;
};