#pragma once

#include <iostream>
#include <vector>
#include "imgui.h"
#include "Core.h"
#include "Core/Airfields.h"
#include "PilotData.h"

struct Frontlines {
	std::string side;
	bool bIsPocket = false;
	std::vector<Vec3> points;

	Frontlines() = default;
};

struct MajorBattle {
	std::string battleId;
	DateTime startDate;
	DateTime endDate;
	Vec3 worldLocation;
	float radius;
};

struct Phase {
	std::string name;
	DateTime startDate;
	std::string description;
	std::vector<ValueStartDate> frontLinesPaths;
	std::vector<MajorBattle> battles;
	Phase() = default;
};

struct Service {
	std::string id;
	std::string icon;
	std::string side;
	std::string nationality;
	std::string picture;
	int country = 0;

	std::vector<Rank> Ranks;

	Service() = default;
};

struct SunTimes {
	std::string sunrise;
	std::string sunset;
};

struct MapLocation {
	std::string id;
	Vec3 worldLocation;
};

enum class ECloudCoverLevel {
	CLEAR,
	LIGHT,
	MEDIUM,
	HEAVY,
	OVERCAST
};

enum class ERainType {
	NONE,
	RAIN,
	SNOW,
};

struct MapData {
	std::string id;
	std::string HMap;
	std::string Textures;
	std::string Forests;
	std::string GuiMap;
	std::string SeasonPrefix;
};

struct WindProfile {
	std::vector<std::pair<float, float>> windLevels; // direction (deg), speed (m/s)
	float turbulence; // 0 (calm) - 10 (very turbulent)
};

struct WeatherInfo {
	MapData mapData;
	float airTemp = 0;
	float airPressure = 760; // normally 700 - 1040
	float haze = 0.0f; //0.0 - 1.0
	WindProfile wind;
	int cloudBase = 1100;
	int cloudTop = 6000;
	std::string cloudConfig;
	int rainType = 0; // 0 = none, 1 = rain, 2 = snow
	int rainAmount = 0; // 0-9
	int seaState = 0; //0-6

	std::string GetWeatherReport()
	{
		std::string report;
		report = std::to_string(int(airTemp)) + " degrees - ";
		if (cloudConfig.substr(0, 2) == "00")
		{
			report += "Clear skies, no rain";
		}
		else if (cloudConfig.substr(0, 2) == "01")
		{
			report += "Mostly clear";
			if (rainAmount > 5)
			{
				report += ", with some scattered rain";
			}
			if (rainAmount > 1)
			{
				report += ", with light patches of rain";
			}
		}
		else if (cloudConfig.substr(0, 2) == "02")
		{
			report += "Scattered cloud";
			if (rainAmount > 5)
			{
				report += ",with patches of rain";
			}
			if (rainAmount > 1)
			{
				report += ",with light patches of rain";
			}
		}
		else if (cloudConfig.substr(0, 2) == "03")
		{
			report += "Cloudy";
			if (rainAmount > 7)
			{
				report += ", with heavy rain";
			}
			else if (rainAmount > 5)
			{
				report += ", with some rain";
			}
			else if (rainAmount > 1)
			{
				report += ", with light rain";
			}
		}
		else if (cloudConfig.substr(0, 2) == "04")
		{
			report += "Overcast";
			if (rainAmount > 7)
			{
				report += ", with heavy rain";
			}
			else if (rainAmount > 5)
			{
				report += ", with some rain";
			}
			else if (rainAmount > 1)
			{
				report += ", with light rain";
			}
		}

		report += " " + std::to_string(int(wind.windLevels[0].second)) + "m/s wind from the " + Core::GetCardinalDirection(wind.windLevels[0].first);

		return report;
	}
};

struct ClimateData {
	std::string season;
	float tempMax = 0;
	float tempMin = 0;
	std::vector<float> cloudCoverChances;
	float preciptChance = 0;
	std::vector<float> precipitationAmountCurve;
	float windStrengthMax = 0;
	std::vector<float> WindStengthChance; 
	float hazeMax = 0;
	float hazeMin = 0;
};

class Theater 
{
public:
	Theater();

	void LoadTheaterFromFile(const std::string& filePath);
	static Theater LoadTheaterFromId(int TheaterIndex);

	void MergeSquadrons(std::vector<Squadron>& baseSquadrons, const std::vector<Squadron>& loadedSquadrons);

	void LoadSquadronsFromTheater();

	Squadron& GetSquadronsFromID(std::string InputID);

	const Service& GetServiceFromID(std::string InputID);

	Rank GetRankFromID(std::string InputId);

	void LoadAirfields(DateTime time);

	void LoadFrontLines(DateTime date);

	const Airfields* GetAirfields(DateTime time);

	const Phase& GetCurrentPhase(DateTime currentTime);

	std::vector<MajorBattle> GetOngoingBattles(DateTime currentTime);

	//safe function for getting all frontline points
	const std::vector<Frontlines>& GetFrontlines(const DateTime& currentDate);

	std::string name;
	std::vector<Phase> phases;
	ImVec2 MapTextureSize;
	ImVec2 MapWorldSize;
	std::string MapTexturePath;
	std::vector<Service> Services;
	std::string path;
	std::vector<std::string> missionTemplates;
	std::vector<Frontlines> LoadedFrontlines;

	std::vector<Squadron> AllSquadrons;

	int theaterIndex;



	std::vector<MapLocation>& GetMapLocations();

	std::unordered_map<int, SunTimes> SunriseSunsetByMonth;

	const ClimateData& GetClimateData(DateTime currentDate);

	WeatherInfo GenerateWeather(DateTime currentDate);

	//returns a std::pair< side string, distance >
	static std::pair<std::string, float> GetPointSideFromFrontlines(const Vec3& point, const std::vector<Frontlines>& allFrontlines);

	MapLocation& GetNearestMapLocation(Vec3 worldPos);

private:

	WindProfile GenerateRandomWindProfile(float windMax, unsigned seed = std::random_device{}());

	int EstimateSeaState(float windSpeed, int rainStrength);

	float EstimateAirPressure(float temperatureC, int cloudCover);
	
	void LoadMapLocations();

	MapData GetMapData(std::string name);

	std::vector<MapLocation> mapLocations;

	std::unordered_map<int, ClimateData> LoadedClimateData;

	std::vector<MapData> mapData;

	std::pair<int, int> mainCountries;

	Airfields TheaterAirfields;
};