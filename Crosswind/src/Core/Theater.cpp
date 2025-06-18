#include "Theater.h"
#include "tinyxml2/tinyxml2.h"
#include "Core/EnumHelpers.h"

Theater::Theater()
{
}

void Theater::LoadTheaterFromFile(const std::string& filePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError result = doc.LoadFile(filePath.c_str());
	if (result != tinyxml2::XML_SUCCESS) {
		std::cerr << "Failed to load theater file: " << doc.ErrorStr() << std::endl;
		return;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Theater");
	if (!root) {
		std::cerr << "No <Theater> root element found." << std::endl;
		return;
	}

	const char* theaterName = root->Attribute("name");
	name = theaterName ? theaterName : "Unknown";

	int newTheaterIndex = 0;
	root->QueryIntAttribute("index", &newTheaterIndex);
	theaterIndex = newTheaterIndex;

	tinyxml2::XMLElement* PathElm = root->FirstChildElement("path");
	if (!PathElm) {
		std::cerr << "No <Path> section found." << std::endl;
		return;
	}
	path = PathElm->GetText();

	tinyxml2::XMLElement* phasesElem = root->FirstChildElement("Phases");
	if (!phasesElem) {
		std::cerr << "No <Phases> section found." << std::endl;
		return;
	}

	tinyxml2::XMLElement* MapsizeElm = root->FirstChildElement("MapTextureSize");
	if (!MapsizeElm) {
		std::cerr << "No <MapTextureSize> section found." << std::endl;
		return;
	}
	MapsizeElm->QueryFloatAttribute("width", &MapTextureSize.x);
	MapsizeElm->QueryFloatAttribute("height", &MapTextureSize.y);

	tinyxml2::XMLElement* MapPathElm = root->FirstChildElement("MapTexturePath");
	if (!MapPathElm) {
		std::cerr << "No <MapTexturePath> section found." << std::endl;
		return;
	}
	MapTexturePath = MapPathElm->GetText();

	tinyxml2::XMLElement* MapWorldsizeElm = root->FirstChildElement("WorldMapSize");
	if (!MapWorldsizeElm) {
		std::cerr << "No <WorldMapSize> section found." << std::endl;
		return;
	}
	MapWorldsizeElm->QueryFloatAttribute("width", &MapWorldSize.x);
	MapWorldsizeElm->QueryFloatAttribute("height", &MapWorldSize.y);

	for (tinyxml2::XMLElement* phaseElem = phasesElem->FirstChildElement("Phase");
		phaseElem != nullptr;
		phaseElem = phaseElem->NextSiblingElement("Phase")) {

		Phase phase;

		const char* nameAttr = phaseElem->Attribute("name");
		phase.name = nameAttr ? nameAttr : "Unnamed Phase";

		tinyxml2::XMLElement* startDateElem = phaseElem->FirstChildElement("StartDate");
		if (startDateElem && startDateElem->GetText())
			phase.startDate = DateTime::FromStrings(startDateElem->GetText());

		tinyxml2::XMLElement* descElem = phaseElem->FirstChildElement("Description");
		if (descElem && descElem->GetText())
			phase.description = descElem->GetText();

		tinyxml2::XMLElement* frontlinesElm = phaseElem->FirstChildElement("Frontlines");
		if (!frontlinesElm) {
			std::cerr << "No <Frontlines> section found." << std::endl;
			return;
		}

		for (tinyxml2::XMLElement* UpdateElem = frontlinesElm->FirstChildElement("Update");
			UpdateElem != nullptr;
			UpdateElem = frontlinesElm->NextSiblingElement("Update")) {

			ValueStartDate NewValue;
			NewValue.startDate = DateTime::FromStrings(UpdateElem->Attribute("date"));
			NewValue.value = UpdateElem->Attribute("file");

			phase.frontLinesPaths.push_back(NewValue);
		}

		tinyxml2::XMLElement* battlesElm = phaseElem->FirstChildElement("Battles");
		if (!battlesElm) {
			std::cerr << "No <Battles> section found." << std::endl;
			return;
		}

		for (tinyxml2::XMLElement* battleElem = battlesElm->FirstChildElement("Battle");
			battleElem != nullptr;
			battleElem = battleElem->NextSiblingElement("Battle")) {

			MajorBattle NewValue;
			NewValue.startDate = DateTime::FromStrings(battleElem->Attribute("startDate"), battleElem->Attribute("startTime"));
			NewValue.endDate = DateTime::FromStrings(battleElem->Attribute("endDate"), battleElem->Attribute("endTime"));
			NewValue.battleId = battleElem->Attribute("id");
			battleElem->QueryFloatAttribute("x", &NewValue.worldLocation.x);
			battleElem->QueryFloatAttribute("y", &NewValue.worldLocation.y);
			battleElem->QueryFloatAttribute("z", &NewValue.worldLocation.z);
			battleElem->QueryFloatAttribute("radius", &NewValue.radius);

			phase.battles.push_back(NewValue);
		}


		phases.push_back(phase);
	}

	tinyxml2::XMLElement* servicesElem = root->FirstChildElement("Services");
	if (!phasesElem) {
		std::cerr << "No <Services> section found." << std::endl;
		return;
	}

	for (tinyxml2::XMLElement* serviceElem = servicesElem->FirstChildElement("AirService");
		serviceElem != nullptr;
		serviceElem = serviceElem->NextSiblingElement("AirService")) {
		
		Service AirService;

		const char* id = serviceElem->Attribute("id");
		AirService.id = id ? id : "NoID";

		const char* side = serviceElem->Attribute("side");
		AirService.side = side ? side : "InvalidSide";

		const char* icon = serviceElem->Attribute("icon");
		AirService.icon = icon ? icon : "NoIcon";

		const char* nationality = serviceElem->Attribute("nationality");
		AirService.nationality = nationality ? nationality : "NoNationality";

		const char* picture = serviceElem->Attribute("pictures");
		AirService.picture = picture ? picture : "Nopicture";

		tinyxml2::XMLElement* ranksElem = serviceElem->FirstChildElement("Ranks");
		for (tinyxml2::XMLElement* rankElem = ranksElem->FirstChildElement("Rank");
			rankElem != nullptr;
			rankElem = rankElem->NextSiblingElement("Rank")) {

			Rank NewRank;
			NewRank.id = rankElem->Attribute("id");
			NewRank.name = rankElem->Attribute("name");
			NewRank.abbrevation = rankElem->Attribute("abbreviation");
			int rankLevel = 0;
			rankElem->QueryIntAttribute("level", &rankLevel);
			NewRank.level = rankLevel;
			int rankScoreReq = 0;
			rankElem->QueryIntAttribute("minScoreRequirement", &rankScoreReq);
			NewRank.minScoreReq = rankScoreReq;

			AirService.Ranks.push_back(NewRank);
		}


		Services.push_back(AirService);
	}

	tinyxml2::XMLElement* sunRiseElem = root->FirstChildElement("SunriseSunsetTable");
	for (tinyxml2::XMLElement* monthElem = sunRiseElem->FirstChildElement("Month"); monthElem; monthElem = monthElem->NextSiblingElement("Month")) {
		int num;
		monthElem->QueryIntAttribute("number", &num);
			std::string sunrise = monthElem->FirstChildElement("Sunrise")->GetText();
			std::string sunset = monthElem->FirstChildElement("Sunset")->GetText();
			
			SunTimes newTime;
			newTime.sunrise = sunrise;
			newTime.sunset = sunset;

			SunriseSunsetByMonth[num] = newTime;
	}

	tinyxml2::XMLElement* climatesElem = root->FirstChildElement("ClimateData");
	for (tinyxml2::XMLElement* monthElem = climatesElem->FirstChildElement("Month"); monthElem; monthElem = monthElem->NextSiblingElement("Month")) {
		int num;
		monthElem->QueryIntAttribute("number", &num);

		ClimateData newClimateDate;
		newClimateDate.season = monthElem->FirstChildElement("Season")->GetText();
		auto* tempElem = monthElem->FirstChildElement("Temp");
		tempElem->QueryFloatAttribute("min", &newClimateDate.tempMin);
		tempElem->QueryFloatAttribute("max", &newClimateDate.tempMax);

		auto* cloudElem = monthElem->FirstChildElement("CloudCover");
		float cloudChance = 0;
		cloudElem->QueryFloatAttribute("clear", &cloudChance);
		newClimateDate.cloudCoverChances.push_back(cloudChance);
		cloudElem->QueryFloatAttribute("light", &cloudChance);
		newClimateDate.cloudCoverChances.push_back(cloudChance);
		cloudElem->QueryFloatAttribute("medium", &cloudChance);
		newClimateDate.cloudCoverChances.push_back(cloudChance);
		cloudElem->QueryFloatAttribute("heavy", &cloudChance);
		newClimateDate.cloudCoverChances.push_back(cloudChance);
		cloudElem->QueryFloatAttribute("overcast", &cloudChance);
		newClimateDate.cloudCoverChances.push_back(cloudChance);

		auto* rainElem = monthElem->FirstChildElement("Rain");
		float rain = 0;
		rainElem->QueryFloatAttribute("chance", &rain);
		newClimateDate.preciptChance = rain;
		rainElem->QueryFloatAttribute("amountLow", &rain);
		newClimateDate.precipitationAmountCurve.push_back(rain);
		rainElem->QueryFloatAttribute("amountMed", &rain);
		newClimateDate.precipitationAmountCurve.push_back(rain);
		rainElem->QueryFloatAttribute("amountHeavy", &rain);
		newClimateDate.precipitationAmountCurve.push_back(rain);

		auto* windElem = monthElem->FirstChildElement("Wind");
		float wind = 0;
		windElem->QueryFloatAttribute("strengthMax", &wind);
		newClimateDate.windStrengthMax = wind;
		windElem->QueryFloatAttribute("amountLow", &wind);
		newClimateDate.WindStengthChance.push_back(wind);
		windElem->QueryFloatAttribute("amountMed", &wind);
		newClimateDate.WindStengthChance.push_back(wind);
		windElem->QueryFloatAttribute("amountHeavy", &wind);
		newClimateDate.WindStengthChance.push_back(wind);

		auto* HazeElem = monthElem->FirstChildElement("Haze");
		HazeElem->QueryFloatAttribute("min", &newClimateDate.hazeMin);
		HazeElem->QueryFloatAttribute("max", &newClimateDate.hazeMax);

		LoadedClimateData[num] = newClimateDate;
	}

	tinyxml2::XMLElement* mapDataElem = root->FirstChildElement("MapData");
	for (tinyxml2::XMLElement* SeasonElem = mapDataElem->FirstChildElement("Season"); SeasonElem; SeasonElem = SeasonElem->NextSiblingElement("Season")) {
		MapData newMapData;
		newMapData.id = SeasonElem->Attribute("name");
		newMapData.HMap = SeasonElem->FirstChildElement("HMap")->GetText();
		newMapData.Textures = SeasonElem->FirstChildElement("Textures")->GetText();
		newMapData.Forests = SeasonElem->FirstChildElement("Forests")->GetText();
		newMapData.GuiMap = SeasonElem->FirstChildElement("GuiMap")->GetText();
		newMapData.SeasonPrefix = SeasonElem->FirstChildElement("SeasonPrefix")->GetText();

		mapData.push_back(newMapData);
	}
	return;
}

void Theater::LoadSquadronsFromTheater()
{
	tinyxml2::XMLDocument doc;
	std::string filePath = path + ("Squadrons.xml");
	tinyxml2::XMLError result = doc.LoadFile(filePath.c_str());
	if (result != tinyxml2::XML_SUCCESS) {
		std::cerr << "Failed to load Squadrons.xml file: " << doc.ErrorStr() << std::endl;
		return;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Squadrons");
	if (!root) {
		std::cerr << "No <Squadrons> root element found." << std::endl;
		return;
	}

	for (tinyxml2::XMLElement* SquadronElem = root->FirstChildElement("Squadron");
		SquadronElem != nullptr;
		SquadronElem = SquadronElem->NextSiblingElement("Squadron")) {

		Squadron NewSquadron;

		NewSquadron.id = SquadronElem->Attribute("id");
		
		NewSquadron.name = SquadronElem->Attribute("name");
		NewSquadron.side = SquadronElem->Attribute("side");
		NewSquadron.service = SquadronElem->Attribute("service");
		float NewExperience = 0.5f;
		SquadronElem->QueryFloatAttribute("Experience", &NewExperience);
		NewSquadron.experience = NewExperience;
		NewSquadron.type = SquadronElem->FirstChildElement("Type")->GetText();

		NewSquadron.iconPath = SquadronElem->FirstChildElement("icon")->GetText();

		tinyxml2::XMLElement* AircraftsElem = SquadronElem->FirstChildElement("Aircraft");
		if (!AircraftsElem) {
			std::cerr << "No <Aircraft> element found." << std::endl;
			return;
		}

		for (tinyxml2::XMLElement* AircraftElem = AircraftsElem->FirstChildElement("Type");
			AircraftElem != nullptr;
			AircraftElem = AircraftsElem->NextSiblingElement("Type")) {

			ValueStartDate NewAircraft;
			NewAircraft.value = AircraftElem->Attribute("name");
			NewAircraft.startDate = DateTime::FromStrings(AircraftElem->Attribute("date"));

			NewSquadron.aircraft.push_back(NewAircraft);
		}

		tinyxml2::XMLElement* AirfieldsElem = SquadronElem->FirstChildElement("Relocations");
		if (!AirfieldsElem) {
			std::cerr << "No <Relocations> element found." << std::endl;
			return;
		}

		for (tinyxml2::XMLElement* AirfieldElem = AirfieldsElem->FirstChildElement("Move");
			AirfieldElem != nullptr;
			AirfieldElem = AirfieldsElem->NextSiblingElement("Move")) {

			ValueStartDate NewAirfield;
			NewAirfield.value = AirfieldElem->Attribute("airfield");
			NewAirfield.startDate = DateTime::FromStrings(AirfieldElem->Attribute("date"));

			NewSquadron.locations.push_back(NewAirfield);
		}

		tinyxml2::XMLElement* MissionTypesElem = SquadronElem->FirstChildElement("MissionTypes");
		if (!MissionTypesElem) {
			std::cerr << "No <MissionTypeslocations> element found." << std::endl;
			return;
		}

		for (tinyxml2::XMLElement* MissionsElem = MissionTypesElem->FirstChildElement("Missions");
			MissionsElem != nullptr;
			MissionsElem = MissionsElem->NextSiblingElement("Missions")) {

			std::pair<DateTime, std::vector<SquadronMissionType>> newMissionData;

			newMissionData.first = DateTime::FromStrings(MissionsElem->Attribute("date"));

			for (tinyxml2::XMLElement* MissionElem = MissionsElem->FirstChildElement("Mission");
				MissionElem != nullptr;
				MissionElem = MissionElem->NextSiblingElement("Mission")) {

				SquadronMissionType newMissionType;

				newMissionType.missionType = Crosswind::FromString<EMissionType>(MissionElem->Attribute("type"));
				MissionElem->QueryFloatAttribute("chance",&newMissionType.missionChance);

				newMissionData.second.push_back(newMissionType);
			}

			NewSquadron.missionTypes.push_back(newMissionData);
		}

		AllSquadrons.push_back(NewSquadron);
	}
}

Squadron& Theater::GetSquadronsFromID(std::string InputID)
{
	static Squadron defaultSquadron{};
	for (int i = 0; i < AllSquadrons.size(); ++i) 
	{
		if (AllSquadrons[i].id == InputID)
		{
			return AllSquadrons[i];
		}

	}
	
	return defaultSquadron;
}

const Service& Theater::GetServiceFromID(std::string InputID)
{
	static const Service defaultService{};
	for (int i = 0; i < Services.size(); ++i)
	{
		if (Services[i].id == InputID)
		{
			return Services[i];
		}

	}
	return defaultService;
}

Rank Theater::GetRankFromID(std::string InputId)
{

	for (const auto& ser : Services)
	{
		for (const auto& r : ser.Ranks)
		{
			if (r.id == InputId)
			{
				return r;
			}
		}
	}

	std::cerr << "Could not find rank from id " << InputId << std::endl;
	return Rank();
}

void Theater::LoadAirfields()
{
	std::string AirfieldsPath = path + ("Airfields.xml");
	TheaterAirfields.LoadAirfields(AirfieldsPath);
}

void Theater::LoadFrontLines(DateTime date)
{
	const auto* CurrentPhase = Core::GetActiveEntryRef<Phase>(
		phases,
		date,
		[](const Phase& e) { return e.startDate; }
	);

	const auto* CurrentFrontlines = Core::GetActiveEntryRef<ValueStartDate>(
		CurrentPhase->frontLinesPaths,
		date,
		[](const ValueStartDate& e) { return e.startDate; }
	);

	std::string frontlinesPath = path + CurrentFrontlines->value;

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(frontlinesPath.c_str()) != tinyxml2::XML_SUCCESS) {
		std::cerr << "Failed to load XML file: " << frontlinesPath.c_str() << std::endl;
		return;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Frontlines");
	if (!root) return;

	for (tinyxml2::XMLElement* Line = root->FirstChildElement("Frontline"); Line != nullptr; Line = Line->NextSiblingElement("Frontline")) {
		Frontlines NewFrontline;
		NewFrontline.side = Line->Attribute("side");
		bool bIsPocketValue = false;
		Line->QueryBoolAttribute("pocket", &bIsPocketValue);
		NewFrontline.bIsPocket = bIsPocketValue;

		for (tinyxml2::XMLElement* point = Line->FirstChildElement("point"); point != nullptr; point = point->NextSiblingElement("point")) {
			Vec3 p;
			point->QueryFloatAttribute("x", &p.x);
			point->QueryFloatAttribute("y", &p.y);
			point->QueryFloatAttribute("z", &p.z);

			NewFrontline.points.push_back(p);
		}

		LoadedFrontlines.push_back(NewFrontline);
	}
}

const Airfields* Theater::GetAirfields()
{
	if (TheaterAirfields.LoadedAirfields.size() == 0)
		LoadAirfields();

	return &TheaterAirfields;
}

const Phase& Theater::GetCurrentPhase(DateTime currentTime)
{
	const auto* CurrentPhase = Core::GetActiveEntryRef<Phase>(
		phases,
		currentTime,
		[](const Phase& e) { return e.startDate; }
	);

	return *CurrentPhase;
}

std::vector<MajorBattle> Theater::GetOngoingBattles(DateTime currentTime)
{
	std::vector<MajorBattle> returnBattles;

	for (const auto& battle : GetCurrentPhase(currentTime).battles)
	{
		if (DateTime::IsOngoing(currentTime, battle.startDate, battle.endDate))
		{
			returnBattles.push_back(battle);
		}
	}
	return returnBattles;
}

const std::vector<Frontlines>& Theater::GetFrontlines(const DateTime& currentDate)
{
	if (LoadedFrontlines.size() == 0)
		LoadFrontLines(currentDate);

		return LoadedFrontlines;
}

std::vector<MapLocation>& Theater::GetMapLocations()
{
	if (mapLocations.size() == 0)
	{
		LoadMapLocations();
	}

	return mapLocations;
}

const ClimateData& Theater::GetClimateData(DateTime currentDate)
{
	return LoadedClimateData[currentDate.month];
}

WeatherInfo Theater::GenerateWeather(DateTime currentDate)
{
	WeatherInfo newWeather;
	const ClimateData& climateData = GetClimateData(currentDate);

	int airTemp = Core::GetWeightedRandomIntFromRange(climateData.tempMin, climateData.tempMax, 0.2, 1, 0.2);
	newWeather.airTemp = airTemp;
	
	int rainWeight = Core::GetWeightedRandomIntFromRange(1, 9, climateData.precipitationAmountCurve[0], climateData.precipitationAmountCurve[1], climateData.precipitationAmountCurve[2]);
	bool isRaining = Core::GetWeightedRandomBool(climateData.preciptChance);
	if (!isRaining)
	{
		newWeather.rainType = 0;
		newWeather.rainAmount = 0;
		rainWeight = 0;
	}
	else
	{
		if (airTemp < -5)
		{
			newWeather.rainType = 2;
		}
		else if (airTemp < 3)
		{
			if (Core::GetWeightedRandomBool(0.5))
			{
				newWeather.rainType = 2;
			}
			else
			{
				newWeather.rainType = 1;
			}
		}
		else
		{
			newWeather.rainType = 1;
		}
		newWeather.rainAmount = rainWeight;
	}

	int cloudCoverValue = Core::GetWeightedRandomIndex(climateData.cloudCoverChances);
	int randCloudNum = Core::GetRandomInt(0, 9);
	std::string configValue;
	switch (cloudCoverValue)
	{
	case 0: 
		configValue = "clear";
		break;
	case 1:
		configValue = "light";
		break;
	case 2:
		configValue = "medium";
		break;
	case 3:
		configValue = "heavy";
		break;
	case 4:
		configValue = "overcast";
		break;
	}

	newWeather.cloudBase = (Core::GetWeightedRandomIntFromRange(75, 160, 0.1, 0.8, 0.2))*10;
	newWeather.cloudTop = (Core::GetWeightedRandomIntFromRange(400, 800, 0.1, 0.8, 0.2))*10;
	newWeather.haze = Core::GetWeightedRandomIntFromRange(climateData.hazeMin * 10, climateData.hazeMax * 10, 0.1, 1, 0.1) * 0.1f;

	std::string configPrefix = (climateData.season == "WINTER" || (climateData.season == "AUTUMN" && airTemp > 3)) ? "winter" : "summer";
	std::string cloudConfigPath = configPrefix + "\\0" + std::to_string(cloudCoverValue) + "_" + configValue + "_0" + std::to_string(randCloudNum) + "\\sky.ini";
	newWeather.cloudConfig = cloudConfigPath;

	if (climateData.season == "WINTER")
	{
		newWeather.mapData = GetMapData("winter");
	}
	else if (climateData.season == "AUTUMN")
	{
		newWeather.mapData = GetMapData("autumn");
	}
	else if (climateData.season == "SUMMER")
	{
		newWeather.mapData = GetMapData("summer");
	}
	else if (climateData.season == "SPRING")
	{
		newWeather.mapData = GetMapData("spring");
	}
	newWeather.airPressure = EstimateAirPressure(airTemp, cloudCoverValue);
	
	float windSpeedMax = (Core::GetWeightedRandomIntFromRange(0, climateData.windStrengthMax * 10, 0.8, 0.4, 0.1)) * 0.1f;
	newWeather.wind = GenerateRandomWindProfile(windSpeedMax);
	newWeather.seaState = EstimateSeaState(windSpeedMax, rainWeight);
	return newWeather;
}

std::pair<std::string,float> Theater::GetPointSideFromFrontlines(const Vec3& point, const std::vector<Frontlines>& allFrontlines)
{
	float closestDistance = std::numeric_limits<float>::max();
	std::string closestSide;

	for (const Frontlines& front : allFrontlines)
	{
		for (size_t i = 0; i < front.points.size() - 1; ++i)
		{
			float dist = Core::DistanceToSegment2D(point, front.points[i], front.points[i + 1]);
			if (dist < closestDistance)
			{
				closestDistance = dist;
				closestSide = front.side;
			}
		}
	}
	std::pair<std::string, float> returnValue = { closestSide,closestDistance };
	return returnValue; // e.g. "ALLIED" or "AXIS"
}

MapLocation& Theater::GetNearestMapLocation(Vec3 worldPos)
{
	float shortestDistance = 9999999.0f;
	MapLocation closestLocation;
	if (mapLocations.size() > 0)
	{
		for (const auto& loc : mapLocations)
		{
			float elementDistance = Core::GetShortestDistance(worldPos, loc.worldLocation);
			if (elementDistance < shortestDistance)
			{
				shortestDistance = elementDistance;
				closestLocation = loc;
			}
		}
	}

	return closestLocation;
}

WindProfile Theater::GenerateRandomWindProfile(float windMax, unsigned seed)
{
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> dirDist(0.0f, 360.0f);        // base direction
	std::uniform_real_distribution<float> speedNoise(-1.5f, 1.5f);      // speed variance
	std::uniform_real_distribution<float> dirNoise(-10.0f, 10.0f);      // direction shear
	std::uniform_real_distribution<float> turbDist(0.0f, 2.0f);         // turbulence noise

	WindProfile profile;
	std::vector<int> altitudes = { 500, 1000, 2000, 5000 };
	profile.windLevels.reserve(altitudes.size());

	float baseDirection = dirDist(rng);
	float baseSpeed = 1.0f + std::uniform_real_distribution<float>(0.0f, windMax)(rng); // m/s near ground

	float lastDirection = baseDirection;
	float lastSpeed = baseSpeed;
	float turbulenceSum = 0.0f;

	for (int i = 0; i < altitudes.size(); ++i)
	{
		float altitudeFactor = 1.0f + (altitudes[i] / 2000.0f); // wind generally stronger at higher altitudes
		float speed = std::max(0.0f, lastSpeed * altitudeFactor + speedNoise(rng));
		float direction = std::fmod(lastDirection + dirNoise(rng) + 360.0f, 360.0f);

		profile.windLevels.emplace_back(direction, speed);

		// Accumulate shear for turbulence estimate
		if (i > 0)
		{
			float dirDiff = std::fabs(direction - profile.windLevels[i - 1].first);
			float speedDiff = std::fabs(speed - profile.windLevels[i - 1].second);
			turbulenceSum += (dirDiff / 25.0f + speedDiff); // weighted shear effect
		}

		lastDirection = direction;
		lastSpeed = speed;
	}

	profile.turbulence = std::min(10.0f, turbulenceSum / (altitudes.size() - 1) + turbDist(rng));
	return profile;
}

int Theater::EstimateSeaState(float windSpeed, int rainStrength)
{
	// Clamp rain strength between 0 and 9
	rainStrength = std::clamp(rainStrength, 0, 9);

	// Base sea state from wind speed
	int seaState = 0;
	if (windSpeed < 0.3f)       seaState = 0;
	else if (windSpeed < 1.6f)  seaState = 1;
	else if (windSpeed < 3.4f)  seaState = 2;
	else if (windSpeed < 5.5f)  seaState = 3;
	else if (windSpeed < 8.0f)  seaState = 4;
	else if (windSpeed < 10.8f) seaState = 5;
	else                        seaState = 6;

	// Increase sea state slightly based on rain strength
	float rainFactor = rainStrength / 9.0f; // 0.0 to 1.0
	seaState += static_cast<int>(rainFactor * 1.5f); // adds 0–1

	return std::clamp(seaState, 0, 6);
}

float Theater::EstimateAirPressure(float temperatureC, int cloudCover)
{
	// Clamp cloud cover between 0 (clear) and 4 (overcast)
	cloudCover = std::clamp(cloudCover, 0, 4);

	// Base pressure at sea level in mmHg (standard is ~760 mmHg)
	float basePressure = 760.0f;

	// Temperature effect: warm air tends to reduce pressure slightly
	float tempEffect = -0.2f * (temperatureC - 15.0f); // drop ~0.2 mmHg per °C above 15

	// Cloud cover effect: more clouds generally mean lower pressure (weather front)
	float cloudEffect = -1.5f * cloudCover; // ~1.5 mmHg drop per cloud level

	float randomEffect = Core::GetRandomInt(-10, 10);
	// Final pressure
	float pressure = basePressure + tempEffect + cloudEffect + randomEffect;

	// Clamp to realistic range (e.g., 720 - 780 mmHg for weather systems)
	return std::clamp(pressure, 720.0f, 780.0f);
}

void Theater::LoadMapLocations()
{
	tinyxml2::XMLDocument doc;
	std::string filePath = path + ("MapLocations.xml");
	tinyxml2::XMLError result = doc.LoadFile(filePath.c_str());
	if (result != tinyxml2::XML_SUCCESS) {
		std::cerr << "Failed to load MapLocations.xml file: " << doc.ErrorStr() << std::endl;
		return;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Locations");
	if (!root) {
		std::cerr << "No <Locations> root element found." << std::endl;
		return;
	}

	for (tinyxml2::XMLElement* locationElem = root->FirstChildElement("Location");
		locationElem != nullptr;
		locationElem = locationElem->NextSiblingElement("Location")) {

		MapLocation newLocation;
		newLocation.id = locationElem->Attribute("id");
		auto* posElem = locationElem->FirstChildElement("Position");
		Vec3 locPos;
		posElem->QueryFloatAttribute("x", &locPos.x);
		posElem->QueryFloatAttribute("y", &locPos.y);
		posElem->QueryFloatAttribute("z", &locPos.z);
		newLocation.worldLocation = locPos;

		mapLocations.push_back(newLocation);
	}
}

MapData Theater::GetMapData(std::string name)
{
	for (auto& m : mapData)
	{
		if (m.id == name)
		{
			return m;
		}
	}
	return MapData();
}

Theater Theater::LoadTheaterFromId(int TheaterIndex)
{
	Theater NewTheater;
	switch (TheaterIndex)
	{
	case 0:
		NewTheater.LoadTheaterFromFile("data/Theaters/Stalingrad/Stalingrad.xml");
		return NewTheater;
		break;
	default:
		return Theater();
		break;
	}
}

void Theater::MergeSquadrons(std::vector<Squadron>& baseSquadrons, const std::vector<Squadron>& loadedSquadrons)
{
	for (Squadron& base : baseSquadrons)
	{
		auto it = std::find_if(loadedSquadrons.begin(), loadedSquadrons.end(),
			[&base](const Squadron& sq) { return sq.id == base.id; });

		if (it == loadedSquadrons.end())
			continue;

		const Squadron& loaded = *it;

		// Merge fields only if the base ones are blank or default
		if (base.name.empty()) base.name = loaded.name;
		if (base.side.empty()) base.side = loaded.side;
		if (base.service.empty()) base.service = loaded.service;
		if (base.iconPath.empty()) base.iconPath = loaded.iconPath;
		if (!base.bSquadronInitialized) base.bSquadronInitialized = loaded.bSquadronInitialized;

		// If base vectors are empty, copy loaded ones
		if (base.Pilots.empty()) base.Pilots = loaded.Pilots;
		if (base.activeAircraft.empty()) base.activeAircraft = loaded.activeAircraft;
		if (base.currentDaysMissions.empty()) base.currentDaysMissions = loaded.currentDaysMissions;
	}
}
