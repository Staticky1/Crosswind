#pragma once

#include "WindowContexts.h"
#include "Core/CampaignData.h"

struct MissionContext : public WindowContext {
    MissionContext(CampaignData& data, int& selectedPilotIndex, SquadronMission& inMission)
        : campaignData(data)
        , selectedPlayerPilotIndex(selectedPilotIndex)
        , mission(inMission)
    {}

    CampaignData& campaignData;
    int& selectedPlayerPilotIndex;
    SquadronMission& mission;
};