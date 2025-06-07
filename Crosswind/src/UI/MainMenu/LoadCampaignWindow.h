#pragma once

#include "UI/CrosswindWindow.h"
#include "Core.h"


class LoadCampaignWindow : public CrosswindWindow
{
public:
	LoadCampaignWindow();

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

private:

	bool m_LoadButtonPressed = false;
	bool m_CancelButtonPressed = false;

	int m_SaveItemSelectedIndex = -1;
	std::vector<std::string> m_campaignNames;
};