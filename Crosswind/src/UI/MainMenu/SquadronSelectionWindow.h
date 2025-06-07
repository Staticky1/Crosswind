#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/CampaignSetupContext.h"
#include "UI/CampaignMapWidget.h"
#include "UI/CrosswindUI.h"

class SelectSquadronWindow : public CrosswindWindow
{
public:
	SelectSquadronWindow(std::shared_ptr<WindowContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::unique_ptr<CrosswindWindow> CreateNextWindow(std::shared_ptr<WindowContext> context) override;
	std::unique_ptr<CrosswindWindow> CreateBackWindow(std::shared_ptr<WindowContext> context) override;

	std::shared_ptr<CampaignSetupContext> m_Context;

private:

	void SelectSquadron(const Squadron* selectedSquadron);

	CampaignSetupData& m_CampaignData;

	CampaignMapWidget MapWidget;

	std::vector<ListItem> ServiceItems;
	std::vector<ListItem> AllSquadrons;

	int SelectedServiceIndex = 0;
	int Prev_SelectedServiceIndex = -1;


	int SelectedSquadronIndex = 0;
	std::string Prev_SelectedSquadronID;

	bool m_Initilized = false;
};