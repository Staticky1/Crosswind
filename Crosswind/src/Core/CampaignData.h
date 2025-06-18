#pragma once

#include "Core.h"
#include "Theater.h"

struct CampaignData
{
	std::string CampaignName;
	
	DateTime CurrentDateTime;

	WeatherInfo CurrentWeather;

	Theater CurrentTheater = Theater();

	std::vector<PilotData> PlayerPilots;

	std::string ExpectedCombatReport;
};