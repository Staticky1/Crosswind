#pragma once

#include "Core.h"
#include "Theater.h"

struct CampaignData
{
	std::string CampaignName;
	
	std::string CurrentDate;
	std::string CurrentTime;

	Theater CurrentTheater = Theater();

	std::vector<PilotData> PlayerPilots;
};