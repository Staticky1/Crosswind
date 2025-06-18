#pragma once
#include "WindowContexts.h"
#include "Core/CampaignData.h"

struct CampaignContext : public WindowContext {
    CampaignContext(CampaignData& data, int& selectedPilotIndex)
        : campaignData(data)
        , selectedPlayerPilotIndex(selectedPilotIndex)
    {}

    CampaignData& campaignData;
    int& selectedPlayerPilotIndex;
};