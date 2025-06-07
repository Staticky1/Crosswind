#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/NewPilotContext.h"

class CreateNewCampaignPopupWindow : public CrosswindWindow
{
public:
	CreateNewCampaignPopupWindow(std::shared_ptr<WindowContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::unique_ptr<CrosswindWindow> CreateNextWindow(std::shared_ptr<WindowContext> context) override;
	std::unique_ptr<CrosswindWindow> CreateBackWindow(std::shared_ptr<WindowContext> context) override;

	std::shared_ptr<NewPilotContext> m_Context;

private:

	CampaignSetupData& m_CampaignData;
	PilotData& m_PilotData;

	ImVec2 DesiredWindowSize = ImVec2(300, 250);

	bool bIsLoading = true;

	bool bHasCreatedSaveData = false;
	bool bHasLoadedData = false;
};