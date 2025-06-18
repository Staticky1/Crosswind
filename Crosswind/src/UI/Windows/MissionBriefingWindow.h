#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/MissionContext.h"
#include "Campaign/CampaignManager.h"

class MissionBriefingWindow : public CrosswindWindow
{
public:
public:
	MissionBriefingWindow(std::shared_ptr<MissionContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::shared_ptr<MissionContext> m_Context;

	void NextStage();
	void PreviousStage();

private:
	CampaignData& m_CampaignData;
	int& m_SelectedPlayerPilotIndex;
	SquadronMission& m_Mission;

	int briefingStage = 0; 

	void DisplayWindProfile(const WindProfile& wind);
};