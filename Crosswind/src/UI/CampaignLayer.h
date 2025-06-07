#pragma once

#include "CrosswindLayer.h"

class CampaignLayer : public CrosswindLayer
{
public:
	void OnUIRender() override;

	void OnAttach() override;

	void OnUpdate(float DeltaTime) override;

private:

	int m_SelectedTabIndex = 0;

};