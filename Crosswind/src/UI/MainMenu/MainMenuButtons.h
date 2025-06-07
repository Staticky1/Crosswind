#pragma once

#include "UI/CrosswindWindow.h"
#include "Core.h"


class MainMenuButtons : public CrosswindWindow
{
public:
	MainMenuButtons();

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	bool bNewCampaignPressed = false;
	bool bLoadCampaignPressed = false;
	bool bSettingsPressed = false;
	bool bExitPressed = false;

	bool bShowButtons = true;

private:


};