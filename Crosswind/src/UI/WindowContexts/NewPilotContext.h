#pragma once

#include "WindowContexts.h"
#include "UI/MainMenu/CampaignSetupData.h"
#include "Core/PilotData.h"

struct NewPilotContext : public WindowContext {
    NewPilotContext(CampaignSetupData& data, PilotData& inPilotData)
        : campaignData(data)
        , pilotData(inPilotData)
    {}

    CampaignSetupData& campaignData;
    PilotData& pilotData;
};