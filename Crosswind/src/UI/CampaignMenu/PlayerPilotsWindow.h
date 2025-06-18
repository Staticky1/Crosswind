#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/CampaignContext.h"

class PlayerPilotWindow : public CrosswindWindow
{
public:
	PlayerPilotWindow(std::shared_ptr<CampaignContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::shared_ptr<CampaignContext> m_Context;

private:
	CampaignData& m_CampaignData;

	std::shared_ptr<Walnut::Image> m_PilotImage = nullptr;

	int& m_SelectedPlayerPilotIndex;

	void CreateVictoryDisplay(std::string displayID, std::string prefix, ImVec2 size, const PilotData& pilotData);
};