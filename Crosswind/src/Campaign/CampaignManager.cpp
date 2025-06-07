#include "CampaignManager.h"
#include "Core/EnumHelpers.h"
#include "NewPilot.h"
#include <random>
#include <filesystem>
#include "Core/CampaignData.h"
#include "Core/CrosswindApp.h"

CampaignManager& CampaignManager::Instance()
{
    static CampaignManager instance;
    return instance;
}

bool CampaignManager::CreateNewCampaignSave(const CampaignSetupData& setupData, const PilotData& playerPilot)
{
    m_CurrentCampaignName = setupData.NewCampaignName;
    CampaignSetupData initilizedData = setupData;
    InitSquadrons(initilizedData, playerPilot);

    std::string path = BuildCampaignSavePath(m_CurrentCampaignName);

    tinyxml2::XMLDocument doc;

    auto* root = doc.NewElement("Campaign");
    doc.InsertFirstChild(root);

    root->SetAttribute("name", initilizedData.NewCampaignName.c_str());
    root->SetAttribute("side", initilizedData.bAlliesSelected ? "Allies" : "Axis");


    // Date and Time
    root->SetAttribute("currentDate", initilizedData.StartDate.c_str());
    auto* timeElem = doc.NewElement("currentTime");
    timeElem->SetText("12:00"); // or dynamic time logic
    root->InsertEndChild(timeElem);

    auto* theaterElm = doc.NewElement("Theater");
    theaterElm->SetAttribute("name", initilizedData.LoadedTheater.name.c_str());
    theaterElm->SetAttribute("index", initilizedData.LoadedTheater.theaterIndex);
    auto* squadronsElm = doc.NewElement("Squadrons");

    for (const auto& s : initilizedData.LoadedTheater.AllSquadrons)
    {
        auto* squadronElm = doc.NewElement("Squadron");
        squadronElm->SetAttribute("id", s.id.c_str());
        squadronElm->SetAttribute("name", s.name.c_str());
        squadronElm->SetAttribute("side", s.side.c_str());
        squadronElm->SetAttribute("service", s.service.c_str());
        squadronElm->SetAttribute("icon", s.iconPath.c_str());
        squadronElm->SetAttribute("initialized", s.bSquadronInitialized ? "true" : "false");

        //pilots
        auto* pilotsElem = doc.NewElement("Pilots");
        for (const auto& p : s.Pilots)
        {
            auto* pilotElm = doc.NewElement("Pilot");
            pilotElm->SetAttribute("playerPilot", p.bIsPlayerPilot ? "true" : "false");
            pilotElm->SetAttribute("name", p.PilotName.c_str());
            pilotElm->SetAttribute("rank", p.Rank.id.c_str());
            pilotElm->SetAttribute("score", p.Score);
            pilotElm->SetAttribute("missions", p.Missions);
            pilotElm->SetAttribute("female", p.bFemalePilot ? "true" : "false");
            pilotElm->SetAttribute("icon", p.IconPath.c_str());
            pilotElm->SetAttribute("status", Crosswind::ToString<EPilotStatus>(p.PilotStatus));
            pilotElm->SetAttribute("wounded", p.woundDuration);
            auto* bg = doc.NewElement("Background");
            bg->SetText(p.Background.c_str());
            pilotElm->InsertEndChild(bg);

            auto* vics = doc.NewElement("Victories");
            for (const auto& v : p.Victories)
            {
                auto* vic = doc.NewElement("Victory");
                vic->SetAttribute("type", Crosswind::ToString<EVictoryType>(v.type));
                vic->SetAttribute("itemName", v.itemName.c_str());
                vic->SetAttribute("date", v.date.c_str());
                vic->SetAttribute("location", v.location.c_str());
                vics->InsertEndChild(vic);
            }
            pilotElm->InsertEndChild(vics);
            pilotsElem->InsertEndChild(pilotElm);
        }
        squadronElm->InsertEndChild(pilotsElem);
        

        auto* aircraftsElem = doc.NewElement("Aircrafts");
        for (const auto& a : s.activeAircraft)
        {
            auto* aircraftElem = doc.NewElement("Aircraft");
            aircraftElem->SetAttribute("type", a.type.c_str());
            aircraftElem->SetAttribute("marking", a.code.c_str());
            aircraftElem->SetAttribute("skin", a.skin.c_str());
            aircraftElem->SetAttribute("status", Crosswind::ToString<ESquadronAircraftStatus>(a.status));
            aircraftElem->SetAttribute("maintanceDuration", std::to_string(a.maintanceDuration).c_str());
            auto* modsElem = doc.NewElement("modifications");
            for (const auto& mod : a.modifications)
            {
                auto* modElem = doc.NewElement("modification");
                modElem->SetText(mod.c_str());
                modsElem->InsertEndChild(modElem);
            }
            aircraftElem->InsertEndChild(modsElem);
            aircraftsElem->InsertEndChild(aircraftElem);
        }
        squadronElm->InsertEndChild(aircraftsElem);

        squadronsElm->InsertEndChild(squadronElm);
    }

    theaterElm->InsertEndChild(squadronsElm);
    root->InsertEndChild(theaterElm);
    // Optional: serialize other campaign elements...

    tinyxml2::XMLError result = doc.SaveFile(path.c_str());
    return result == tinyxml2::XML_SUCCESS;
}

const std::string& CampaignManager::GetCurrentCampaignName() const
{
    return m_CurrentCampaignName;
}

std::string CampaignManager::BuildCampaignSavePath(const std::string& campaignName) const
{
    return "data/saves/" + campaignName + ".xml";
}

void CampaignManager::InitSquadrons(CampaignSetupData& setupData, const PilotData& playerPilot)
{
    for (auto& squadron : setupData.LoadedTheater.AllSquadrons)
    {
        if (!squadron.bSquadronInitialized)
        {
            //Create Pilots
            int numberOfPilots = Core::GetWeightedRandomIntFromRange(8, 15, 0.1, 1, 0.1);

            std::vector<Rank> serviceRanks = setupData.LoadedTheater.GetServiceFromID(squadron.service).Ranks;
            int maxRanks = serviceRanks.size();
            std::unordered_map<int, int> currentRankCounts;
            std::unordered_map<int, int> maxPerLevel = {
                {maxRanks - 2, 5},  // e.g. only 2 pilots at level 5
                {maxRanks - 1, 3},  // only 1 Major/Captain/etc.
                {maxRanks, 1}
            };

            std::vector<int> preassigned;
            AssignMandatoryHighRanks(serviceRanks, 2, currentRankCounts, maxPerLevel, preassigned); //ensure there are at least 2 high ranking pilots in each squadron

            for (int index = 0; index < numberOfPilots; ++index)
            {
                Rank selectedRank;
                if (index <= 1)
                {
                    int rankIndex = preassigned[index];
                    selectedRank = serviceRanks[rankIndex];
                }
                else
                {
                    int rankIndex = SelectCappedRankIndex(serviceRanks, squadron.experience, currentRankCounts, maxPerLevel);
                    selectedRank = serviceRanks[rankIndex];
                    currentRankCounts[selectedRank.level]++;
                }

                PilotData newPilotData;
                newPilotData.Rank = selectedRank;
                CreateNewPilot(setupData.LoadedTheater.GetServiceFromID(squadron.service), squadron, setupData.StartDate, newPilotData);

                squadron.Pilots.push_back(newPilotData);
            }

            //Create Aircraft

            if (m_AircraftList.size() == 0)
                LoadAircraftDataFromXML("data/aircraft/Aircraft.xml");

            int codeNumber = 0;

            if (squadron.service == "VVS" || squadron.service == "Normandie")
            {
                codeNumber = (Core::GetWeightedRandomIntFromRange(1, 8, 0.1, 1, 0.1) * 10);
            }

            int numberOfAircraft = Core::GetWeightedRandomIntFromRange(9, 19, 0.1, 1, 0.1);
            
            const auto* aircraftType = Core::GetActiveEntryRef<ValueStartDate>(
                squadron.aircraft,
                setupData.StartDate,
                [](const ValueStartDate& e) { return e.startDate; }
            );

            const AircraftData* aircraftData = GetAircraftByType(aircraftType->value);

            for (int i = 0; i < numberOfAircraft; ++i)
            {
                SquadronAircraft newAircraft;

                newAircraft.type = aircraftType->value;

                ++codeNumber;
                if (Core::GetWeightedRandomBool(0.2)) ++codeNumber; //skip a number

                if (aircraftData->tacCodeType == "NUMBER")
                {
                    newAircraft.code = std::to_string(codeNumber);
                }
                if (aircraftData->tacCodeType == "LETTER")
                {
                    newAircraft.code = std::string(1, static_cast<char>('A' + codeNumber));
                }

                squadron.activeAircraft.push_back(newAircraft);
            }


            squadron.bSquadronInitialized = true;
        }
    }

    //Add player pilot
    Squadron& playerSquadron = setupData.LoadedTheater.GetSquadronsFromID(playerPilot.SquadronId);
    playerSquadron.Pilots.push_back(playerPilot);
}

void CampaignManager::CreateNewPilot(const Service& Service, const Squadron& Squadron, std::string CurrentDate, PilotData& pilotData)
{
    if (Service.id == "VVS")
    {
        pilotData.bFemalePilot = Core::GetWeightedRandomBool(0.03) ? true : false;
    }
    else
    {
        pilotData.bFemalePilot = false;
    }
    
    pilotData.PilotName = Crosswind::GetName(Service.nationality, pilotData.bFemalePilot);
    //pilotData.Background = Crosswind::GetBackground(Service.id,pilotData.PilotName,true,pilotData.bFemalePilot);
    pilotData.IconPath = Crosswind::GetPilotImage(Service.picture,pilotData.bFemalePilot);
    
    int minScore = pilotData.Rank.minScoreReq;
    if (Service.id == "VVS" && Core::HasDatePassed(CurrentDate, "1943-01-6")) minScore -= 20; //order No.5 - soviet pilots start two ranks higher
    int pilotRankLevel = pilotData.Rank.level;
    int maxScore = 0;
    for (auto& rank : Service.Ranks)
    {
        if (rank.level == 1 + pilotRankLevel)
        {
            maxScore = rank.minScoreReq + (rank.minScoreReq * 0.5f);
            break;
        }
    }

    if (maxScore == 0)
    {
        maxScore = minScore * 1.25f;
    }

    pilotData.Score = Core::GetWeightedRandomIntFromRange(minScore, maxScore, 0.5, 0.5, 0.2);
    if (pilotData.Score > 1)
        pilotData.Missions = pilotData.Score - Core::GetWeightedRandomIntFromRange(0, (pilotData.Score * 0.4f) + 1, 0.3, 1.0, 0.1);
    else
        pilotData.Missions = 0;

    //create previous victory stats

    int numberOfVictories = Core::GetWeightedRandomIntFromRange((pilotData.Score - pilotData.Missions) * 0.1f, (pilotData.Score - pilotData.Missions) + 1, 0.5, 1.0, 0.1);

    std::vector<PilotVictory> victories;

    for (int i = 0; i < numberOfVictories; ++i)
    {
        float isAircraftChance = Squadron.type == "Fighter" ? 0.3f : 0.05f;
        
        if (Service.id == "Luftwaffe" && Squadron.type == "Fighter") //luftwaffe less likely to have fighters conducting ground attack missions
            isAircraftChance = 0.66f;

        PilotVictory NewVictory;

        std::vector<EVictoryType> possibleTypes;

        if (Core::GetWeightedRandomBool(isAircraftChance))
        {
            std::vector<EVictoryType> aircraftVictories = { AIRCRAFT_LIGHT, AIRCRAFT_MEDIUM  };
            possibleTypes.insert(possibleTypes.end(), aircraftVictories.begin(), aircraftVictories.end());

            if (Core::GetWeightedRandomBool(0.5f))
            {
                possibleTypes.push_back(AIRCRAFT_HEAVY);
            }

            ++i; //air victories mean overall less victories.
        }
        else
        {
            float marineChance = 0.f;
            if (Core::GetWeightedRandomBool(marineChance))
            {
                std::vector<EVictoryType> marineVictories = { MARINE_LIGHT,MARINE_CARGO };
                if (Core::GetWeightedRandomBool(0.2f))
                {
                    marineVictories.push_back(MARINE_DESTROYER);
                    marineVictories.push_back(MARINE_SUBMARINE);
                }
                
                possibleTypes.insert(possibleTypes.end(), marineVictories.begin(), marineVictories.end());
            }

            if (Squadron.type == "Bomber")
            {
                std::vector<EVictoryType> buildingVictories = { BUILDING_FACILITY, BUILDING_RESIDENTIAL };
                possibleTypes.insert(possibleTypes.end(), buildingVictories.begin(), buildingVictories.end());
            }

            if (Core::GetWeightedRandomBool(0.5f))
            {
                //high value targets
                possibleTypes.push_back(RAIL_LOCOMOTIVE);
                possibleTypes.push_back(RAIL_STATION);
                possibleTypes.push_back(BUILDING_BRIDGE);
                possibleTypes.push_back(VEHICLE_HEAVYARMOR);
                possibleTypes.push_back(WEAPON_ROCKETLAUNCHER);
            }

            std::vector<EVictoryType> otherVictories = { AIRCRAFT_PARKED, VEHICLE_TRANSPORT, VEHICLE_LIGHTARMOR, VEHICLE_MEDIUMARMOR, RAIL_RAILCAR, WEAPON_MG, WEAPON_CANNON, WEAPON_AAA };
            possibleTypes.insert(possibleTypes.end(), otherVictories.begin(), otherVictories.end());
        }

        NewVictory.type = GetRandomVictoryType(possibleTypes);
        victories.push_back(NewVictory);
    }

    pilotData.Victories = victories;
}

int CampaignManager::SelectCappedRankIndex(const std::vector<Rank>& ranks, float experienceModifier, std::unordered_map<int, int>& currentCounts, const std::unordered_map<int, int>& maxAllowedPerLevel)
{
    std::vector<double> weights;
    for (const Rank& rank : ranks)
    {
        // Check if this rank level has reached its cap
        int level = rank.level;
        int current = currentCounts[level];
        auto maxIt = maxAllowedPerLevel.find(level);
        if (maxIt != maxAllowedPerLevel.end() && current >= maxIt->second) {
            weights.push_back(0.0); // eliminate this option
            continue;
        }

        // Bias weight by experience modifier
        float bias = 2.0f - experienceModifier;
        double weight = 1.0 / std::pow(level, bias);
        weights.push_back(weight);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    return dist(gen); // returns index into `ranks`
}

void CampaignManager::AssignMandatoryHighRanks(const std::vector<Rank>& ranks, int numHighRanksNeeded, std::unordered_map<int, int>& currentCounts, const std::unordered_map<int, int>& maxAllowedPerLevel, std::vector<int>& preassignedIndices)
{
    std::vector<int> highRankLevels;

    int highRankThreshold = ranks.size() - 2; //top two ranks

    for (const Rank& r : ranks)
    {
        if (r.level >= highRankThreshold) // what is considered "high rank"
            highRankLevels.push_back(r.level);
    }

    std::sort(highRankLevels.begin(), highRankLevels.end(), std::greater<>());

    int assigned = 0;
    for (int level : highRankLevels)
    {
        auto maxIt = maxAllowedPerLevel.find(level);
        if (maxIt == maxAllowedPerLevel.end()) continue;

        int available = maxIt->second - currentCounts[level];
        int toAssign = std::min(numHighRanksNeeded - assigned, available);

        for (int i = 0; i < toAssign; ++i)
        {
            // Find index of this level in ranks
            for (int j = 0; j < ranks.size(); ++j)
            {
                if (ranks[j].level == level)
                {
                    preassignedIndices.push_back(j);
                    currentCounts[level]++;
                    assigned++;
                    break;
                }
            }

            if (assigned >= numHighRanksNeeded)
                return;
        }
    }
}

EVictoryType CampaignManager::GetRandomVictoryType(std::vector<EVictoryType> possibleTypes)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, possibleTypes.size() - 1);
    return possibleTypes[dist(gen)];
}

bool CampaignManager::LoadAircraftDataFromXML(const std::string& filename) {
    using namespace tinyxml2;

    std::vector<AircraftData> aircraftList;
    XMLDocument doc;

    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filename << "\n";
        return false;
    }

    XMLElement* root = doc.FirstChildElement("planes");
    if (!root) {
        std::cerr << "Missing <planes> root element.\n";
        return false;
    }

    for (XMLElement* planeElem = root->FirstChildElement("plane"); planeElem; planeElem = planeElem->NextSiblingElement("plane")) {
        AircraftData aircraft;
        aircraft.type = planeElem->Attribute("type") ? planeElem->Attribute("type") : "";

        auto getText = [](XMLElement* elem) {
            return elem && elem->GetText() ? std::string(elem->GetText()) : "";
            };

        aircraft.archType = getText(planeElem->FirstChildElement("archType"));
        aircraft.side = getText(planeElem->FirstChildElement("side"));
        aircraft.displayName = getText(planeElem->FirstChildElement("displayName"));
        aircraft.script = getText(planeElem->FirstChildElement("script"));
        aircraft.model = getText(planeElem->FirstChildElement("model"));
        aircraft.desc = getText(planeElem->FirstChildElement("desc"));
        aircraft.planeSize = getText(planeElem->FirstChildElement("planeSize"));
        aircraft.introduction = getText(planeElem->FirstChildElement("introduction"));
        aircraft.endProduction = getText(planeElem->FirstChildElement("endProduction"));
        aircraft.withdrawal = getText(planeElem->FirstChildElement("withdrawal"));
        aircraft.tacCodeType = getText(planeElem->FirstChildElement("tacCodeType"));

        if (XMLElement* e = planeElem->FirstChildElement("cruisingSpeed"))
            e->QueryIntText(&aircraft.cruisingSpeed);
        if (XMLElement* e = planeElem->FirstChildElement("climbOutRate"))
            e->QueryIntText(&aircraft.climbOutRate);
        if (XMLElement* e = planeElem->FirstChildElement("goodness"))
            e->QueryIntText(&aircraft.goodness);
        if (XMLElement* e = planeElem->FirstChildElement("range"))
            e->QueryIntText(&aircraft.range);
        if (XMLElement* e = planeElem->FirstChildElement("isFlyable"))
            e->QueryBoolText(&aircraft.isFlyable);
        if (XMLElement* e = planeElem->FirstChildElement("isNovice"))
            e->QueryBoolText(&aircraft.isNovice);

        // Countries
        if (auto usedByElem = planeElem->FirstChildElement("primaryUsedBy")) {
            for (XMLElement* c = usedByElem->FirstChildElement("country"); c; c = c->NextSiblingElement("country")) {
                aircraft.primaryUsedBy.emplace_back(getText(c));
            }
        }

        // Modifications
        if (auto modsElem = planeElem->FirstChildElement("stockModifications")) {
            for (XMLElement* m = modsElem->FirstChildElement("modification"); m; m = m->NextSiblingElement("modification")) {
                Modification mod;
                mod.name = m->Attribute("name") ? m->Attribute("name") : getText(m);
                if (const char* sd = m->Attribute("startDate")) mod.startDate = sd;
                if (const char* ed = m->Attribute("endDate")) mod.endDate = ed;
                aircraft.stockModifications.emplace_back(mod);
            }
        }

        // Roles
        if (auto rolesElem = planeElem->FirstChildElement("roleCategories")) {
            for (XMLElement* r = rolesElem->FirstChildElement("role"); r; r = r->NextSiblingElement("role")) {
                aircraft.roleCategories.emplace_back(getText(r));
            }
        }

        aircraftList.emplace_back(aircraft);
    }

    m_AircraftList = aircraftList;
    return true;
}

const std::vector<AircraftData>& CampaignManager::GetAircraftList() const
{
    return m_AircraftList;
}

const AircraftData* CampaignManager::GetAircraftByType(const std::string& id) const
{
    for (const auto& ac : m_AircraftList) {
        if (ac.type == id)
            return &ac;
    }
    return nullptr;
}

bool CampaignManager::LoadCampaignSaveFile(const std::string& FileName)
{
    std::string filePath = "data/saves/" + FileName + ".xml";

    if (std::filesystem::exists(filePath))
    {
        std::vector<Squadron> allSquadrons;
        std::string date;
        std::string time;
        int campaignIndex = 0;
        std::string newName = FileName;
        if (CampaignManager::Instance().LoadCampaignSave(filePath, allSquadrons, date, time, campaignIndex))
        {
            CampaignData newData;
            Theater newTheater = Theater::LoadTheaterFromId(campaignIndex);
            newTheater.LoadSquadronsFromTheater();
            newData.CampaignName = newName;
            newData.CurrentDate = date;
            newData.CurrentTime = time;

            newTheater.MergeSquadrons(newTheater.AllSquadrons, allSquadrons);
            newData.CurrentTheater = newTheater;

            Core::Instance().GetApp()->SetCampaignData(newData);
            return true;
        }
        else
        {
            std::cerr << "Failed to Load Save Data!" << std::endl;
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CampaignManager::LoadCampaignSave(const std::string& xmlFilePath, std::vector<Squadron>& outSquadrons, std::string& outDate, std::string& outTime, int& campaignIndex)
{
    using namespace tinyxml2;

    XMLDocument doc;
    if (doc.LoadFile(xmlFilePath.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load campaign save: " << xmlFilePath << std::endl;
        return false;
    }

    XMLElement* campaignElem = doc.FirstChildElement("Campaign");
    if (!campaignElem) return false;

    outDate = campaignElem->Attribute("currentDate");
    XMLElement* timeElem = campaignElem->FirstChildElement("currentTime");
    if (timeElem && timeElem->GetText())
        outTime = timeElem->GetText();

    XMLElement* theaterElem = campaignElem->FirstChildElement("Theater");
    if (!theaterElem) return false;

    XMLElement* squadronsElem = theaterElem->FirstChildElement("Squadrons");
    if (!squadronsElem) return false;

    for (XMLElement* squadElem = squadronsElem->FirstChildElement("Squadron"); squadElem; squadElem = squadElem->NextSiblingElement("Squadron")) {
        Squadron squad;
        squad.id = squadElem->Attribute("id");
        squad.name = squadElem->Attribute("name");
        squad.side = squadElem->Attribute("side");
        squad.service = squadElem->Attribute("service");
        squad.iconPath = squadElem->Attribute("icon");
        squad.bSquadronInitialized = squadElem->BoolAttribute("initialized", false);

        // -- Pilots --
        XMLElement* pilotsElem = squadElem->FirstChildElement("Pilots");
        if (pilotsElem) {
            for (XMLElement* pilotElem = pilotsElem->FirstChildElement("Pilot"); pilotElem; pilotElem = pilotElem->NextSiblingElement("Pilot")) {
                PilotData pilot;
                pilot.bIsPlayerPilot = pilotElem->BoolAttribute("playerPilot", false);
                pilot.PilotName = pilotElem->Attribute("name");
                pilot.SquadronId = squad.id;
                pilot.ServiceId = squad.service;
                pilot.IconPath = pilotElem->Attribute("icon");
                pilot.bFemalePilot = pilotElem->BoolAttribute("female", false);
                pilot.Score = pilotElem->FloatAttribute("score");
                pilot.Missions = pilotElem->IntAttribute("missions");
                pilot.woundDuration = pilotElem->IntAttribute("wounded");

                // Rank
                pilot.Rank.id = pilotElem->Attribute("rank");

                // Status enum
                pilot.PilotStatus = Crosswind::FromString<EPilotStatus>(pilotElem->Attribute("status"));

                // Background
                XMLElement* bgElem = pilotElem->FirstChildElement("Background");
                if (bgElem && bgElem->GetText()) {
                    pilot.Background = bgElem->GetText();
                }

                // Victories
                XMLElement* victoriesElem = pilotElem->FirstChildElement("Victories");
                if (victoriesElem) {
                    for (XMLElement* victoryElem = victoriesElem->FirstChildElement("Victory"); victoryElem; victoryElem = victoryElem->NextSiblingElement("Victory")) {
                        PilotVictory victory;
                        victory.type = Crosswind::FromString<EVictoryType>(victoryElem->Attribute("type"));
                        victory.itemName = victoryElem->Attribute("itemName");
                        victory.date = victoryElem->Attribute("date");
                        victory.location = victoryElem->Attribute("location");
                        pilot.Victories.push_back(victory);
                    }
                }

                squad.Pilots.push_back(pilot);
            }
        }
        // -- Aircraft --
        XMLElement* aircraftsElem = squadElem->FirstChildElement("Aircrafts");
        if (aircraftsElem) {
            for (XMLElement* acElem = aircraftsElem->FirstChildElement("Aircraft"); acElem; acElem = acElem->NextSiblingElement("Aircraft")) {
                SquadronAircraft aircraft;
                aircraft.type = acElem->Attribute("type");
                aircraft.code = acElem->Attribute("marking");
                aircraft.skin = acElem->Attribute("skin");
                aircraft.status = Crosswind::FromString<ESquadronAircraftStatus>(acElem->Attribute("status"));
                aircraft.maintanceDuration = acElem->IntAttribute("maintanceDuration");

                // Skipping modifications for now (could be populated later if needed)
                squad.activeAircraft.push_back(aircraft);
            }
        }

        outSquadrons.push_back(squad);
    }

    return true;
}
