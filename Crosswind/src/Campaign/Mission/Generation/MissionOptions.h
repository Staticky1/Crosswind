#pragma once

#include "MissionObject.h"

class MissionOptions : public MissionObject
{
public:
	MissionOptions() : MissionObject(EMissionObjectType::Options) {}

	int LCName = 0;
	int LCDesc = 1;
	int LCAuthor = 2;
	std::string PlayerConfig = "";

	std::string Time;
	std::string Date;

	std::string HMap;
	std::string Textures;
	std::string Forests;
	std::string Layers = "";
	std::string GuiMap;
	std::string SeasonPrefix;
	int MissionType = 0; // 0 = single
	int AqmId = 0;

	int CloudLevel = 600;
	int CloudHeight = 4000;
	int PrecLevel = 0;
	int PrecType = 0;
	std::string CloudConfig;
	int SeaState = 0;
	int Turbulence = 1;
	int TempPressLevel = 0;
	int Temperature = 0;
	int Pressure = 760;
	float Haze = 0.5;
	int LayerFog = 0;
	std::vector<std::pair<float, float>> WindLayers;
	std::vector<std::pair<int, int>> Countries;

protected:
	void WriteFields(std::ostream& out) override;
};