#pragma once

#include "WindowContexts.h"
#include "UI/MainMenu/CampaignSetupData.h"

struct CampaignSetupContext : public WindowContext {
    CampaignSetupContext(CampaignSetupData& data)
        : campaignData(data) {}

    CampaignSetupData& campaignData;
};