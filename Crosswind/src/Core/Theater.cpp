#include "Theater.h"
#include "tinyxml2/tinyxml2.h"

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
			phase.startDate = startDateElem->GetText();

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
			NewValue.startDate = UpdateElem->Attribute("date");
			NewValue.value = UpdateElem->Attribute("file");

			phase.frontLinesPaths.push_back(NewValue);
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
			NewAircraft.startDate = AircraftElem->Attribute("date");

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
			NewAirfield.startDate = AirfieldElem->Attribute("date");

			NewSquadron.locations.push_back(NewAirfield);
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

void Theater::LoadAirfields()
{
	std::string AirfieldsPath = path + ("Airfields.xml");
	TheaterAirfields.LoadAirfields(AirfieldsPath);
}

void Theater::LoadFrontLines(std::string date)
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
	}
}
