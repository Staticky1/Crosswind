#pragma once

#include "Walnut/Application.h"
#include "Core/Theater.h"
#include "CampaignMapWidget.h"
#include "UI/CrosswindUI.h"
#include "UI/CrosswindLayer.h"
#include "UI/MainMenu/CampaignSetupData.h"
#include "WindowContexts/CampaignSetupContext.h"
#include "WindowContexts/NewPilotContext.h"


class MainMenuLayer : public CrosswindLayer
{

public:
	void OnUIRender() override;

	void OnAttach() override;

	void OnUpdate(float DeltaTime) override;

	CampaignSetupData CampaignSetup;
	PilotData NewPilot;

private:

	std::shared_ptr<WindowContext> m_ActiveContext;
	std::shared_ptr<CampaignSetupContext> m_CampaignSetupContext;
	std::shared_ptr<NewPilotContext> m_NewPilotContext;

	bool bShowNewCampaignWindow = false;
	bool bShowNewCampaignWindowPart2 = false;

	std::string NewCampaignName = "";

	bool bAlliesSelected = false;
	bool bAxisSelected = false;

	int SelectedTheaterIndex = 0;
	int SelectedPhaseIndex = 0;

	std::string SelectedDate;

	Theater LoadedTheater;

	bool bShowButtons = true;

	CampaignMapWidget MapWidget;


	std::vector<ListItem> ServiceItems;
	std::vector<ListItem> AllSquadrons;

	int SelectedServiceIndex = 0;
	int Prev_SelectedServiceIndex = -1;

	
	int SelectedSquadronIndex = 0;
	std::string Prev_SelectedSquadronID;

};