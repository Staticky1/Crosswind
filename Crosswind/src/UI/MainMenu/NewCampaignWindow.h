#pragma once

#include "UI/CrosswindWindow.h"
#include "Core.h"
#include <iostream>
#include <filesystem>
#include "Core/Theater.h"
#include "UI/MainMenuUI.h"

class NewCampaignWindow : public CrosswindWindow
{
public:
	NewCampaignWindow(std::shared_ptr<WindowContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::unique_ptr<CrosswindWindow> CreateNextWindow(std::shared_ptr<WindowContext> context) override;

	std::shared_ptr<CampaignSetupContext> m_Context;

private:
	
	CampaignSetupData& m_CampaignData;

	bool bNextButtonPressed = false;
	bool bCancelButtonPressed = false;

};