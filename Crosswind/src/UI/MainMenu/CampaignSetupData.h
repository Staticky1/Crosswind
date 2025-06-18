#pragma once
#include "Core.h"
#include "Core/Theater.h"

struct CampaignSetupData
{
	std::string NewCampaignName;
	bool bAlliesSelected = false;
	bool bAxisSelected = false;
	Theater LoadedTheater;

	DateTime StartDate;

	Squadron FinalSelectedSquadron;
	Service FinalSelectedService;

	int SelectedTheaterIndex = 0;
	int SelectedPhaseIndex = 0;
};
