#pragma once

#include "CrosswindLayer.h"
#include "UI/CampaignMenu/SquadronInfoWindow.h"
#include "UI/Windows/MissionBriefingWindow.h"
#include "UI/Windows/NewsPopup.h"
#include "UI/WindowContexts/MissionContext.h"
#include <future>


class CampaignLayer : public CrosswindLayer
{
public:
	void OnUIRender() override;

	void OnAttach() override;

	void OnUpdate(float DeltaTime) override;

	void AddNewsItem(NewsItem& item);

	void ShowNewsItems();

	void ClearNewsItems();

private:
	void StartMissionBuildCommandASync();
	void DrawLoadingPopup();

	bool bShowMissionGenerationLoading = false;
	bool isRunningCommand = false;
	std::future<void> commandFuture;

	std::shared_ptr<CampaignContext> m_CampaignContext;
	std::shared_ptr<MissionContext> m_MissionContext;
	int m_SelectedPlayerPilotIndex = 0;
	SquadronMission* m_CurrentMission = nullptr;
	int m_SelectedTabIndex = 0;

	

	std::unique_ptr<MissionBriefingWindow> m_MissionBriefingWindow;

	NewsPopupWindow newsPopup;
	std::vector<NewsItem> CurrentNewsItems;
	int newsItemIndex;
	bool bShouldShowNews = false;

	bool bShowMissionBriefing = false;

};