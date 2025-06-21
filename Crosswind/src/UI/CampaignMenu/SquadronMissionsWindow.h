#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/CampaignContext.h"
#include "Campaign/CampaignManager.h"

class SquadronMissionsWindow : public CrosswindWindow
{
public:
	SquadronMissionsWindow(std::shared_ptr<CampaignContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::shared_ptr<CampaignContext> m_Context;
	bool bMissionBriefingScreen = false;


private:
	CampaignData& m_CampaignData;

	bool bRequestNewMission = false;

	bool bStartMission = false;


	int& m_SelectedPlayerPilotIndex;

	bool AllMissionComplete(const Squadron& playerSquadron);

	const SquadronMission* GetCurrentMission(const Squadron& playerSquadron);

	void MarkMissionComplete(Squadron& playerSquadron);

	bool MissionContainsPlayer(const SquadronMission& mission);

	std::vector<std::string> BuildMissionResultNews(std::vector<EncounterResult>& results, bool excludeOtherSquads, bool excludeOwnSquad = false);
};