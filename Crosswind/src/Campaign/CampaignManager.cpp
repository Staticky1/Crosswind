#include "CampaignManager.h"
#include "Core/EnumHelpers.h"
#include "NewPilot.h"
#include <random>
#include <filesystem>
#include <set>
#include "Core/CampaignData.h"
#include "Core/CrosswindApp.h"
#include "Core.h"
#include "Core/Theater.h"
#include "UI/CampaignLayer.h"
#include "Core/Theater.h"
#include "Campaign/Mission/Generation/MissionFileWriter.h"

CampaignManager& CampaignManager::Instance()
{
    static CampaignManager instance;
    return instance;
}

void CampaignManager::SetCampaignLayer(CampaignLayer* campaignLayer)
{
    CampaignLayerPtr = campaignLayer;
}

bool CampaignManager::CreateNewCampaignSave(const CampaignSetupData& setupData, const PilotData& playerPilot)
{
    m_CurrentCampaignName = setupData.NewCampaignName;
    CampaignSetupData initilizedData = setupData;
    InitSquadrons(initilizedData, playerPilot);

    CampaignData newCampaignData;
    newCampaignData.CampaignName = m_CurrentCampaignName;
    newCampaignData.CurrentDateTime = initilizedData.StartDate;
    newCampaignData.CurrentTheater = initilizedData.LoadedTheater;
    newCampaignData.ExpectedCombatReport = "";
    newCampaignData.PlayerPilots = std::vector<PilotData>({playerPilot});

   return SaveCampaignData(newCampaignData,0);
}

const std::string& CampaignManager::GetCurrentCampaignName() const
{
    return m_CurrentCampaignName;
}

bool CampaignManager::SaveCampaignData(CampaignData& campaignData, int selectedPilotIndex)
{
    std::string path = BuildCampaignSavePath(campaignData.CampaignName);

    if (std::filesystem::exists(path))
    {
        BackupExistingSave(path);
    }

    tinyxml2::XMLDocument doc;

    auto* root = doc.NewElement("Campaign");
    doc.InsertFirstChild(root);

    root->SetAttribute("name", campaignData.CampaignName.c_str());
    root->SetAttribute("side", campaignData.CurrentTheater.GetServiceFromID(campaignData.PlayerPilots[0].ServiceId).side.c_str());
    root->SetAttribute("version", CROSS_VERSION);

    // Date and Time
    root->SetAttribute("currentDate", campaignData.CurrentDateTime.ToDateString().c_str());
    auto* timeElem = doc.NewElement("currentTime");
    timeElem->SetText(campaignData.CurrentDateTime.ToTimeString().c_str());
    root->InsertEndChild(timeElem);

    using namespace tinyxml2;
    WeatherInfo info = campaignData.CurrentWeather;
    // MapData
    XMLElement* weather = doc.NewElement("Weather");
    XMLElement* map = doc.NewElement("MapData");
    map->SetAttribute("id", info.mapData.id.c_str());
    map->SetAttribute("HMap", info.mapData.HMap.c_str());
    map->SetAttribute("Textures", info.mapData.Textures.c_str());
    map->SetAttribute("Forests", info.mapData.Forests.c_str());
    map->SetAttribute("GuiMap", info.mapData.GuiMap.c_str());
    map->SetAttribute("SeasonPrefix", info.mapData.SeasonPrefix.c_str());
    weather->InsertEndChild(map);

    weather->SetAttribute("airTemp", info.airTemp);
    weather->SetAttribute("airPressure", info.airPressure);
    weather->SetAttribute("haze", info.haze);
    weather->SetAttribute("cloudBase", info.cloudBase);
    weather->SetAttribute("cloudTop", info.cloudTop);
    weather->SetAttribute("cloudConfig", info.cloudConfig.c_str());
    weather->SetAttribute("rainType", info.rainType);
    weather->SetAttribute("rainAmount", info.rainAmount);
    weather->SetAttribute("seaState", info.seaState);

    // Wind
    XMLElement* wind = doc.NewElement("WindProfile");
    wind->SetAttribute("turbulence", info.wind.turbulence);

    for (const auto& [dir, speed] : info.wind.windLevels) {
        XMLElement* level = doc.NewElement("WindLevel");
        level->SetAttribute("direction", dir);
        level->SetAttribute("speed", speed);
        wind->InsertEndChild(level);
    }

    weather->InsertEndChild(wind);
    root->InsertEndChild(weather);
    auto* theaterElm = doc.NewElement("Theater");
    theaterElm->SetAttribute("name", campaignData.CurrentTheater.name.c_str());
    theaterElm->SetAttribute("index", campaignData.CurrentTheater.theaterIndex);
    auto* squadronsElm = doc.NewElement("Squadrons");

    for (const auto& s : campaignData.CurrentTheater.AllSquadrons)
    {
        auto* squadronElm = doc.NewElement("Squadron");
        squadronElm->SetAttribute("id", s.id.c_str());
        squadronElm->SetAttribute("name", s.name.c_str());
        squadronElm->SetAttribute("side", s.side.c_str());
        squadronElm->SetAttribute("service", s.service.c_str());
        squadronElm->SetAttribute("icon", s.iconPath.c_str());
        squadronElm->SetAttribute("initialized", s.bSquadronInitialized ? "true" : "false");

        //Missions
        auto* missionsElem = doc.NewElement("Missions");
        for (const auto& m : s.currentDaysMissions)
        {
            auto* missionElm = doc.NewElement("Mission");
            missionElm->SetAttribute("complete", m.missionComplete);

            //mission info
            auto* missionPlanElm = doc.NewElement("MissionPlan");
            missionPlanElm->SetAttribute("date", m.missionPlan.missionDate.ToDateString().c_str());
            missionPlanElm->SetAttribute("time", m.missionPlan.missionDate.ToTimeString().c_str());

            auto* missionTemplateElm = doc.NewElement("MissionTemplate");
            missionTemplateElm->SetAttribute("type", Crosswind::ToString<EMissionType>(m.missionPlan.missionType.type));
            missionTemplateElm->SetAttribute("name", m.missionPlan.missionType.name.c_str());
            missionTemplateElm->SetAttribute("description", m.missionPlan.missionType.description.c_str());

            missionPlanElm->InsertEndChild(missionTemplateElm);

            auto* missionTargetElm = doc.NewElement("MissionTarget");
            missionTargetElm->SetAttribute("id", m.missionPlan.missionArea.id.c_str());
            missionTargetElm->SetAttribute("control", m.missionPlan.missionArea.control.c_str());
            missionTargetElm->SetAttribute("type", m.missionPlan.missionArea.type.c_str());
            auto* missionLocationElm = doc.NewElement("WorldPosition");
            missionLocationElm->SetAttribute("x", m.missionPlan.missionArea.worldPosition.x);
            missionLocationElm->SetAttribute("y", m.missionPlan.missionArea.worldPosition.y);
            missionLocationElm->SetAttribute("z", m.missionPlan.missionArea.worldPosition.z);
            missionTargetElm->InsertEndChild(missionLocationElm);
            missionPlanElm->InsertEndChild(missionTargetElm);
            auto* missionWaypointsElm = doc.NewElement("Waypoints");
            for (const auto& w : m.missionPlan.missionWaypoints)
            {
                auto* missionWaypointElm = doc.NewElement("Waypoint");
                missionWaypointElm->SetAttribute("name", w.name.c_str());
                missionWaypointElm->SetAttribute("x", w.WorldLocation.x);
                missionWaypointElm->SetAttribute("y", w.WorldLocation.y);
                missionWaypointElm->SetAttribute("z", w.WorldLocation.z);
                missionWaypointElm->SetAttribute("canEdit", w.canBeEdited);
                missionWaypointsElm->InsertEndChild(missionWaypointElm);
            }
            missionPlanElm->InsertEndChild(missionWaypointsElm);

            missionElm->InsertEndChild(missionPlanElm);

            auto* assignedPilotsElm = doc.NewElement("AssignedPilots");
            //assigned pilots
            for (const auto& pilot : m.AssignedPilots)
            {
                auto* assignedPilotElm = doc.NewElement("Pilot");
                assignedPilotElm->SetAttribute("name", pilot.PilotName.c_str());
                assignedPilotElm->SetAttribute("rank", pilot.Rank.id.c_str());
                assignedPilotsElm->InsertEndChild(assignedPilotElm);
            }
            missionElm->InsertEndChild(assignedPilotsElm);

            //assigned aircraft
            auto* assignedAircraftsElm = doc.NewElement("AssignedAircrafts");
            for (const auto& aircraft : m.assignedAircraft)
            {
                auto* assignedAircraftElm = doc.NewElement("Aircraft");
                assignedAircraftElm->SetAttribute("type", aircraft.type.c_str());
                assignedAircraftElm->SetAttribute("code", aircraft.code.c_str());
                assignedAircraftsElm->InsertEndChild(assignedAircraftElm);
            }
            missionElm->InsertEndChild(assignedAircraftsElm);

            missionsElem->InsertEndChild(missionElm);
        }

        squadronElm->InsertEndChild(missionsElem);

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

CampaignLayer* CampaignManager::GetCampaignLayer() const
{
    return CampaignLayerPtr;
}

void CampaignManager::UpdateCampaignData(CampaignData& campaignData, std::vector<EncounterResult>& results)
{
    for (const auto& result: results)
    {
        const PilotData& attackingPilot = result.attackingPilot.first;
        const SquadronAircraft& attackingAircraft = result.attackingPilot.second;
        const PilotData& targetPilot = result.targetPilot.first;
        const SquadronAircraft& targetAircraft = result.targetPilot.second;

        Squadron& attackingSquadron = campaignData.CurrentTheater.GetSquadronsFromID(attackingPilot.SquadronId);
        PilotData& attackingPilotData = attackingSquadron.GetPilotFromNameRank(attackingPilot.PilotName, attackingPilot.Rank.id);
        Squadron& targetSquadron = campaignData.CurrentTheater.GetSquadronsFromID(targetPilot.SquadronId);
        PilotData& targetPilotData = targetSquadron.GetPilotFromNameRank(targetPilot.PilotName, targetPilot.Rank.id);

        int targetAircraftIndex = 0;
        for (int i = 0; i < targetSquadron.activeAircraft.size(); ++i)
        {
            if (targetSquadron.activeAircraft[i].code == targetAircraft.code && targetSquadron.activeAircraft[i].type == targetAircraft.type)
            {
                targetAircraftIndex = i;
                break;
            }
        }

        if (result.result == EEncounterResult::SHOTDOWN_BAILED || result.result == EEncounterResult::SHOTDOWN_KILLED || result.result == EEncounterResult::SHOTDOWN_CRASHLANDING)
        {
            PilotVictory newVictory;
            newVictory.itemName = targetAircraft.type;
            newVictory.date = campaignData.CurrentDateTime.ToDateString();
            newVictory.location = campaignData.CurrentTheater.GetNearestMapLocation(result.worldLocation).id;
            EVictoryType victoryType = VICTORY_NONE;
            if (GetAircraftByType(targetAircraft.type)->planeSize == "PLANE_SIZE_SMALL") victoryType = EVictoryType::AIRCRAFT_LIGHT;
            if (GetAircraftByType(targetAircraft.type)->planeSize == "PLANE_SIZE_MEDIUM") victoryType = EVictoryType::AIRCRAFT_MEDIUM;
            if (GetAircraftByType(targetAircraft.type)->planeSize == "PLANE_SIZE_LARGE") victoryType = EVictoryType::AIRCRAFT_HEAVY;
            newVictory.type = victoryType;

            attackingPilotData.Victories.push_back(newVictory);

            //find and remove aircraft

            targetSquadron.activeAircraft.erase(targetSquadron.activeAircraft.begin() + targetAircraftIndex);
        }

        if (result.result == EEncounterResult::SHOTDOWN_KILLED)
        {
            targetPilotData.PilotStatus = EPilotStatus::EPILOT_DEAD; //mark pilot as dead
        }
        else
        {
            if (result.pilotInjury > 0)
            {
                targetPilotData.PilotStatus = EPilotStatus::EPILOT_WOUNDED;
                targetPilotData.woundDuration = result.pilotInjury;
            }
            if (result.aircraftMaintance > 0)
            {
                targetSquadron.activeAircraft[targetAircraftIndex].maintanceDuration = result.aircraftMaintance;
                targetSquadron.activeAircraft[targetAircraftIndex].status = ESquadronAircraftStatus::EAIRCRAFT_MAINTANCE;
            }
        }
        
    }
}

int CampaignManager::GeneratePilotInjury(EEncounterResult resultType)
{
    switch (resultType)
    {
    case EEncounterResult::SHOTDOWN_BAILED:
        if (m_InjuryChanceOnBail < Core::RandomFloat(0.0f, 1.0f))
        {
            if (Core::GetWeightedRandomBool(m_MajorInjuryOnBail))
            {
                return Core::GetRandomInt(m_ModerateInjuryMaxDays,m_MajorInjuryMaxDays);
                break;
            }
            else if (Core::GetWeightedRandomBool(m_ModerateInjuryOnBail))
            {
                return Core::GetRandomInt(m_MinorInjuryMaxDays, m_ModerateInjuryMaxDays);
                break;
            }
            else
            {
                return Core::GetRandomInt(1, m_MinorInjuryMaxDays);
                break;
            }
        }
        else
        {
            return 0;
            break;
        }
    case EEncounterResult::SHOTDOWN_CRASHLANDING:
        if (m_InjuryChanceOnCrashLand < Core::RandomFloat(0.0f, 1.0f))
        {
            if (Core::GetWeightedRandomBool(m_MajorInjuryOnCrashLand))
            {
                return Core::GetRandomInt(m_ModerateInjuryMaxDays, m_MajorInjuryMaxDays);
                break;
            }
            else if (Core::GetWeightedRandomBool(m_ModerateInjuryOnCrashLand))
            {
                return Core::GetRandomInt(m_MinorInjuryMaxDays, m_ModerateInjuryMaxDays);
                break;
            }
            else
            {
                return Core::GetRandomInt(1, m_MinorInjuryMaxDays);
                break;
            }
        }
        else
        {
            return 0;
            break;
        }

    case EEncounterResult::DAMAGED_SEVERE:
        if (Core::GetWeightedRandomBool(m_InjuryChanceOnAircraftDamage))
        {
            return Core::GetRandomInt(m_ModerateInjuryMaxDays, m_MajorInjuryMaxDays);
            break;
        }
        else
        {
            return 0;
            break;
        }
    case EEncounterResult::DAMAGED_MEDIUM:
        if (Core::GetWeightedRandomBool(m_InjuryChanceOnAircraftDamage * 0.5f))
        {
            return Core::GetRandomInt(m_MinorInjuryMaxDays, m_ModerateInjuryMaxDays);
            break;
        }
        else
        {
            return 0;
            break;
        }
    case EEncounterResult::DAMAGED_MINOR:
        if (Core::GetWeightedRandomBool(m_InjuryChanceOnAircraftDamage * 0.25f))
        {
            return Core::GetRandomInt(1, m_MinorInjuryMaxDays);
            break;
        }
        else
        {
            return 0;
            break;
        }
    default:
        return 0;
        break;
    }
    

}

int CampaignManager::GenerateAircraftDamage(EEncounterResult resultType)
{
    if (resultType == EEncounterResult::DAMAGED_MINOR)
    {
        return Core::GetRandomInt(1, m_MinorMaintanceMaxDays);
    }
    if (resultType == EEncounterResult::DAMAGED_MEDIUM)
    {
        return Core::GetRandomInt(m_MinorMaintanceMaxDays, m_ModerateMaintanceMaxDays);
    }
    if (resultType == EEncounterResult::DAMAGED_SEVERE)
    {
        return Core::GetRandomInt(m_ModerateMaintanceMaxDays, m_MajorMaintanceMaxDays);
    }
    return 0;
}

void CampaignManager::GenerateMissionFile(CampaignData& inCampaignData, int& SelectedPlayerPilot, SquadronMission& inMission)
{
    MissionFileWriter missionWriter = MissionFileWriter(inCampaignData, SelectedPlayerPilot, inMission);

    for (const auto& temp : inCampaignData.CurrentTheater.missionTemplates)
    {
        missionWriter.ParseStaticObjectXml(inCampaignData.CurrentTheater.path + temp);
    }
    missionWriter.AddAirfields(inCampaignData.CurrentTheater.GetAirfields(inCampaignData.CurrentDateTime));

    missionWriter.WriteMissionFile();
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
            int numberOfPilots = Core::GetWeightedRandomIntFromRange(7, 14, 0.1, 1, 0.1);

            std::vector<Rank> serviceRanks = setupData.LoadedTheater.GetServiceFromID(squadron.service).Ranks;
            int maxRanks = static_cast<int>(serviceRanks.size());
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

            int numberOfAircraft = Core::GetWeightedRandomIntFromRange(9, 14, 0.1, 1, 0.1);
            
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

void CampaignManager::CreateNewPilot(const Service& Service, const Squadron& Squadron,const DateTime& CurrentDate, PilotData& pilotData)
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
    if (Service.id == "VVS" && Core::HasDatePassed(CurrentDate, DateTime::FromStrings("1943-01-6"))) minScore -= 20; //order No.5 - soviet pilots start two ranks higher
    int pilotRankLevel = pilotData.Rank.level;
    int maxScore = 0;
    for (auto& rank : Service.Ranks)
    {
        if (rank.level == 1 + pilotRankLevel)
        {
            maxScore = static_cast<int>(rank.minScoreReq + (rank.minScoreReq * 0.5f));
            break;
        }
    }

    if (maxScore == 0)
    {
        maxScore = minScore * 1.35f;
    }

    pilotData.Score = Core::GetWeightedRandomIntFromRange(minScore, maxScore, 0.5, 0.5, 0.2);
    if (pilotData.Score > 1)
        pilotData.Missions = static_cast<int>(pilotData.Score - Core::GetWeightedRandomIntFromRange(0, round(pilotData.Score * 0.4f) + 1, 0.2, 1.0, 0.5));
    else
        pilotData.Missions = 0;

    //create previous victory stats

    int numberOfVictories = Core::GetWeightedRandomIntFromRange(round((pilotData.Score - pilotData.Missions) * 0.2f), round(pilotData.Score - pilotData.Missions) + 1, 0.1, 0.8, 0.1);

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

            if (Core::GetWeightedRandomBool(0.4f))
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

    int highRankThreshold = static_cast<int>(ranks.size() - 2); //top two ranks

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

    std::unordered_map<std::string,AircraftData> aircraftList;
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

        aircraftList[aircraft.type] = aircraft;
    }

    m_AircraftList = aircraftList;
    return true;
}

const std::unordered_map<std::string, AircraftData>& CampaignManager::GetAircraftList()
{
    if (m_AircraftList.size() == 0)
        LoadAircraftDataFromXML("data/aircraft/Aircraft.xml");
    return m_AircraftList;
}

const AircraftData* CampaignManager::GetAircraftByType(const std::string& id)
{
    if (m_AircraftList.size() == 0)
        LoadAircraftDataFromXML("data/aircraft/Aircraft.xml");
    
    return &m_AircraftList.at(id);
}

bool CampaignManager::LoadCampaignSaveFile(const std::string& FileName)
{
    std::string filePath = "data/saves/" + FileName + ".xml";

    if (std::filesystem::exists(filePath))
    {
        std::vector<Squadron> allSquadrons;
        std::vector<PilotData> playerPilots;
        std::string date;
        std::string time;
        WeatherInfo newWeatherInfo;
        int campaignIndex = 0;
        std::string newName = FileName;
        if (CampaignManager::Instance().LoadCampaignSave(filePath, allSquadrons, date, time, campaignIndex, playerPilots, newWeatherInfo))
        {
            CampaignData newData;
            Theater newTheater = Theater::LoadTheaterFromId(campaignIndex);
            newTheater.LoadSquadronsFromTheater();
            newData.CampaignName = newName;
            newData.CurrentDateTime = DateTime::FromStrings(date,time);
            newData.PlayerPilots = playerPilots;
            newTheater.MergeSquadrons(newTheater.AllSquadrons, allSquadrons);
            newData.CurrentTheater = newTheater;
            newData.CurrentWeather = newWeatherInfo;
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

bool CampaignManager::LoadCampaignSave(const std::string& xmlFilePath, std::vector<Squadron>& outSquadrons, std::string& outDate, std::string& outTime, int& campaignIndex, std::vector<PilotData>& playerPilots, WeatherInfo& info)
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
    float version = -1.0f;
    campaignElem->QueryFloatAttribute("version", &version); //version can be used to update old save data/ensure it can still be loaded

    if (version == -1.0f)
        version = 0.0;

    XMLElement* timeElem = campaignElem->FirstChildElement("currentTime");
    if (timeElem && timeElem->GetText())
        outTime = timeElem->GetText();

    XMLElement* weather = campaignElem->FirstChildElement("Weather");
    XMLElement* map = weather->FirstChildElement("MapData");
    if (map) {
        info.mapData.id = map->Attribute("id") ? map->Attribute("id") : "";
        info.mapData.HMap = map->Attribute("HMap") ? map->Attribute("HMap") : "";
        info.mapData.Textures = map->Attribute("Textures") ? map->Attribute("Textures") : "";
        info.mapData.Forests = map->Attribute("Forests") ? map->Attribute("Forests") : "";
        info.mapData.GuiMap = map->Attribute("GuiMap") ? map->Attribute("GuiMap") : "";
        info.mapData.SeasonPrefix = map->Attribute("SeasonPrefix") ? map->Attribute("SeasonPrefix") : "";
    }

    weather->QueryFloatAttribute("airTemp", &info.airTemp);
    weather->QueryFloatAttribute("airPressure", &info.airPressure);
    weather->QueryFloatAttribute("haze", &info.haze);
    weather->QueryIntAttribute("cloudBase", &info.cloudBase);
    weather->QueryIntAttribute("cloudTop", &info.cloudTop);
    info.cloudConfig = weather->Attribute("cloudConfig") ? weather->Attribute("cloudConfig") : "";
    weather->QueryIntAttribute("rainType", &info.rainType);
    weather->QueryIntAttribute("rainAmount", &info.rainAmount);
    weather->QueryIntAttribute("seaState", &info.seaState);

    // WindProfile
    info.wind.windLevels.clear();
    XMLElement* wind = weather->FirstChildElement("WindProfile");
    if (wind) {
        wind->QueryFloatAttribute("turbulence", &info.wind.turbulence);

        for (XMLElement* level = wind->FirstChildElement("WindLevel"); level; level = level->NextSiblingElement("WindLevel")) {
            float dir = 0.0f, speed = 0.0f;
            level->QueryFloatAttribute("direction", &dir);
            level->QueryFloatAttribute("speed", &speed);
            info.wind.windLevels.emplace_back(dir, speed);
        }
    }

    XMLElement* theaterElem = campaignElem->FirstChildElement("Theater");
    if (!theaterElem) return false;

    theaterElem->QueryIntAttribute("index", &campaignIndex);

    Theater tempTheater = Theater::LoadTheaterFromId(campaignIndex);

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
                std::string rankId = pilotElem->Attribute("rank");
                pilot.Rank = tempTheater.GetRankFromID(rankId);

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

                if (pilot.bIsPlayerPilot)
                {
                    playerPilots.push_back(pilot);
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
        // -- Missions --
        XMLElement* missionsElem = squadElem->FirstChildElement("Missions");
        if (missionsElem) {
            for (XMLElement* missionElem = missionsElem->FirstChildElement("Mission"); missionElem; missionElem = missionElem->NextSiblingElement("Mission"))
            {
                SquadronMission newMission;
                missionElem->QueryBoolAttribute("complete", &newMission.missionComplete);
                XMLElement* missionPlanElem = missionElem->FirstChildElement("MissionPlan");
                newMission.missionPlan.missionDate = DateTime::FromStrings(missionPlanElem->Attribute("date"), missionPlanElem->Attribute("time"));
                XMLElement* missionTemplateElem = missionPlanElem->FirstChildElement("MissionTemplate");
                newMission.missionPlan.missionType.type = Crosswind::FromString<EMissionType>(missionTemplateElem->Attribute("type"));
                newMission.missionPlan.missionType.name = missionTemplateElem->Attribute("name");
                newMission.missionPlan.missionType.description = missionTemplateElem->Attribute("description");

                XMLElement* missionTargetElem = missionPlanElem->FirstChildElement("MissionTarget");
                newMission.missionPlan.missionArea.id = missionTargetElem->Attribute("id");
                newMission.missionPlan.missionArea.control = missionTargetElem->Attribute("control");
                newMission.missionPlan.missionArea.type = missionTargetElem->Attribute("type");
                XMLElement* missionPosElem = missionTargetElem->FirstChildElement("WorldPosition");
                missionPosElem->QueryFloatAttribute("x", &newMission.missionPlan.missionArea.worldPosition.x);
                missionPosElem->QueryFloatAttribute("y", &newMission.missionPlan.missionArea.worldPosition.y);
                missionPosElem->QueryFloatAttribute("z", &newMission.missionPlan.missionArea.worldPosition.z);
                

                XMLElement* missionWaypointsElem = missionPlanElem->FirstChildElement("Waypoints");
                for (XMLElement* waypointElem = missionWaypointsElem->FirstChildElement("Waypoint"); waypointElem; waypointElem = waypointElem->NextSiblingElement("Waypoint"))
                {
                    MissionWaypoint newWaypoint;
                    newWaypoint.name = waypointElem->Attribute("name");
                    waypointElem->QueryFloatAttribute("x", &newWaypoint.WorldLocation.x);
                    waypointElem->QueryFloatAttribute("y", &newWaypoint.WorldLocation.y);
                    waypointElem->QueryFloatAttribute("z", &newWaypoint.WorldLocation.z);
                    waypointElem->QueryBoolAttribute("canEdit", &newWaypoint.canBeEdited);

                    newMission.missionPlan.missionWaypoints.push_back(newWaypoint);
                }

                XMLElement* assignedPilotsElem = missionElem->FirstChildElement("AssignedPilots");
                //mission pilots
                for (XMLElement* pilotElem = assignedPilotsElem->FirstChildElement("Pilot"); pilotElem; pilotElem = pilotElem->NextSiblingElement("Pilot"))
                {
                    //find pilot from rank and name
                    for (const auto& p : squad.Pilots)
                    {
                        if (pilotElem->Attribute("name") == p.PilotName && pilotElem->Attribute("rank") == p.Rank.id)
                        {
                            newMission.AssignedPilots.push_back(p);
                            break;
                        }
                    }                   
                }

                //mission aircraft
                XMLElement* assignedAircraftsElem = missionElem->FirstChildElement("AssignedAircrafts");
                for (XMLElement* aircraftElem = assignedAircraftsElem->FirstChildElement("Aircraft"); aircraftElem; aircraftElem = aircraftElem->NextSiblingElement("Aircraft"))
                {
                    //find aircraft from type and code
                    for (const auto& a : squad.activeAircraft)
                    {
                        if (aircraftElem->Attribute("type") == a.type && aircraftElem->Attribute("code") == a.code)
                        {
                            newMission.assignedAircraft.push_back(a);
                            break;
                        }
                    }
                }

                squad.currentDaysMissions.push_back(newMission);
            }
        }


        outSquadrons.push_back(squad);
    }

    return true;
}

void CampaignManager::BackupExistingSave(const std::string& saveFilePath)
{
    namespace fs = std::filesystem;

    fs::path originalPath(saveFilePath);
    fs::path backupPath = originalPath.parent_path() / (originalPath.stem().string() + "_backup" + originalPath.extension().string());

    try {
        if (fs::exists(originalPath)) {
            fs::copy_file(originalPath, backupPath, fs::copy_options::overwrite_existing);
            std::cout << "Backup created at: " << backupPath << "\n";
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to create backup: " << e.what() << "\n";
    }
}

void CampaignManager::RequestNewDay(DateTime& currentDate, Theater* theater, std::vector<PilotData>& playerPilots)
{
    DateTime newDate;

    currentDate.hour = 0;
    currentDate.minute = 0;
    currentDate.AddDays(1);

    for (const auto& pilot : playerPilots)
    {
        StartNewDay(currentDate, theater, theater->GetSquadronsFromID(pilot.SquadronId));
    }
}

void CampaignManager::SimulateMission(DateTime& missionDate, Theater* theater, SquadronMission& mission, Squadron& playerSquadron, std::vector<EncounterResult>& results)
{
    //Find other squadrons
    Vec3 missionLocation = mission.missionPlan.missionArea.worldPosition;
    std::vector<Squadron> nearbySquadrons = GetNearbySquadrons(missionDate, theater, missionLocation); //squadrons that could be in the mission
    std::vector<std::pair<Squadron, MissionPlan>> selectedSquadrons;
    for (auto& squad : nearbySquadrons)
    {
        if (squad.id != playerSquadron.id && (squad.ReadyPilots() > 2 && squad.ReadyAircraft() > 2))
        {
            MissionPlan newMissionPlan = missionGenerator.CreateMissionPlan(theater, squad, missionDate, Core::Instance().GetApp()->GetCampaignData().CurrentWeather);
            if (Core::GetShortestDistance(newMissionPlan.missionArea.worldPosition, missionLocation) <= 60000)
            {
                std::pair<Squadron, MissionPlan> selection = { squad,newMissionPlan };
                selectedSquadrons.push_back(selection);
            }
        }
    }
    selectedSquadrons.push_back({ playerSquadron,mission.missionPlan }); //add player squadron back in

    //Find encounters between squadrons
    CombatState combatState;
    std::vector<EncounterResult> allEncounterResults;
    std::vector<Encounter> missionEncounters = CheckSquadronEncounters(selectedSquadrons,playerSquadron);
    for (auto& encounter : missionEncounters)
    {
        std::vector<EncounterResult> results;
        SimulateAirCombat(encounter, results, combatState);

        for (const auto& r : results)
        {
            allEncounterResults.push_back(r);
            int squadACount = encounter.squadAPilots.size();
            int squadBCount = encounter.squadBPilots.size();
            int squadANew = 0;
            int squadBNew = 0;
            
            switch (r.result)
            {
            case EEncounterResult::SHOTDOWN_KILLED:
            case EEncounterResult::SHOTDOWN_BAILED:
            case EEncounterResult::SHOTDOWN_CRASHLANDING:
                for (const auto& pilot : encounter.squadAPilots)
                {
                    if (pilot.first.PilotName == r.targetPilot.first.PilotName)
                    {
                        squadANew++;
                    }
                    else
                    {
                        squadBNew++;
                    }
                }

                break;
            case EEncounterResult::DAMAGED_SEVERE:
            case EEncounterResult::DAMAGED_MEDIUM:
            case EEncounterResult::DAMAGED_MINOR:
                combatState.RTBAircraft.insert(r.targetPilot.first.PilotName);
                for (const auto& pilot : encounter.squadAPilots)
                {
                    if (pilot.first.PilotName == r.targetPilot.first.PilotName)
                    {
                        squadANew++;
                    }
                    else
                    {
                        squadBNew++;
                    }
                }
                break;
            }

            if (squadANew / squadACount >= 0.4f)
            {
                combatState.retreatedSquadrons.insert(encounter.squadA->id);
            }
            if (squadBNew / squadBCount >= 0.4f)
            {
                combatState.retreatedSquadrons.insert(encounter.squadB->id);
            }
        }
    }
    results = allEncounterResults;
}

void CampaignManager::UpdateMissionData(Squadron& playerSquadron)
{
    for (auto& mission : playerSquadron.currentDaysMissions)
    {
        if (!mission.missionComplete)
        {
            missionGenerator.UpdateMissionPlan(playerSquadron,mission);
        }
    }
}

//returns a list of squadrons that could reach the area
std::vector<Squadron> CampaignManager::GetNearbySquadrons(const DateTime& currentTime, Theater* theater, Vec3 worldLocation, float maxRangeMod)
{
    std::vector<Squadron> returnSquadrons;

    for (const auto& squadron : theater->AllSquadrons)
    {
        const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
            squadron.locations,
            currentTime,
            [](const ValueStartDate& e) { return e.startDate; }
        );

        const Airfield& squadronAirfield = theater->GetAirfields(currentTime)->LoadedAirfields.at(AirfieldValue->value);
        
        float distance = Core::GetShortestDistance(worldLocation, squadronAirfield.position);
        const auto* AircraftValue = Core::GetActiveEntryRef<ValueStartDate>(
            squadron.aircraft,
            currentTime,
            [](const ValueStartDate& e) { return e.startDate; }
        );
        const AircraftData* aircraftData = GetAircraftByType(AircraftValue->value);
        float maxWaypointRange = aircraftData->range * 500.0f;

        if (distance <= (maxWaypointRange * maxRangeMod))
        {
            returnSquadrons.push_back(squadron);
        }
    }
    
    return returnSquadrons;
}

void CampaignManager::StartNewDay(DateTime newDate, Theater* theater, Squadron& squadron)
{
    //Generate Missions for the day for the players squadron
    //get the amount of mission

    int numberOfMission = Core::GetWeightedRandomIndex(std::vector<double>() = { 0.7, 0.5, 0.1 });
    ++numberOfMission;

    DateTime missionTime;
    SunTimes sunRiseSet = theater->SunriseSunsetByMonth.at(newDate.month);
    auto times = GetMissionTimes(sunRiseSet.sunrise, sunRiseSet.sunset);
    
    if (times.size() < numberOfMission)
        numberOfMission = times.size();

    std::vector<std::string> selectedTimes;

    for (int i = 0; i < numberOfMission; ++i)
    {
        int index = rand() % times.size();
        selectedTimes.push_back(times[index]);
        times.erase(times.begin() + index);
    }
    DateTime::SortTimeStrings(selectedTimes);
    std::vector<SquadronMission> missionPlans;
    std::unordered_map<std::string, int> missionNumberTracker;
    for (int i = 0; i < numberOfMission; ++i)
    {
        SquadronMission newSquadMission;

        int numberOfAvaPilots = 0;
        for (int i = 0; i < squadron.Pilots.size(); ++i)
        {
            if (squadron.Pilots[i].PilotStatus & EPILOT_READY)
            {
                numberOfAvaPilots++;
            }
        }
        int numberOfAvaAircraft = 0;
        for (int i = 0; i < squadron.activeAircraft.size(); ++i)
        {
            if (squadron.activeAircraft[i].status == EAIRCRAFT_READY)
            {
                numberOfAvaAircraft++;
            }
        }

        if (numberOfAvaPilots < 3 || numberOfAvaAircraft < 3)
        {
            //scrap missions because lack of pilots or planes
            squadron.currentDaysMissions.clear();
            return;
        }

        WeatherInfo newWeatherInfo = theater->GenerateWeather(newDate);
        Core::Instance().GetApp()->GetCampaignData().CurrentWeather = newWeatherInfo;

        DateTime newTime = DateTime::FromStrings(newDate.ToDateString(), selectedTimes[i]);
        int numberOfPilots = Core::GetWeightedRandomIntFromRange(3, std::min<int>({ 9, numberOfAvaPilots, numberOfAvaAircraft }), 0.2, 1, 0.2);
        MissionPlan newPlan = missionGenerator.CreateMissionPlan(theater, squadron, newTime, Core::Instance().GetApp()->GetCampaignData().CurrentWeather);
        newSquadMission.missionPlan = newPlan;
        MissionRequirements newReq;
        newReq.numberToSelect = numberOfPilots;
        newReq.randomizeSelection = true;
        std::vector<const PilotData*> assignedPilots = missionGenerator.GetAvailablePilotsForMission(squadron.Pilots, newReq, missionNumberTracker);
        for (const auto& a : assignedPilots)
        {
            newSquadMission.AssignedPilots.push_back(*a);
            missionNumberTracker[a->PilotName]++;
        }

        std::vector<const SquadronAircraft*> assignedAircraft = missionGenerator.GetAvailableAircraftForMission(squadron.activeAircraft, newReq);
        for (const auto& a : assignedAircraft)
        {
            newSquadMission.assignedAircraft.push_back(*a);
        }

        missionPlans.push_back(newSquadMission);
    }
    
    squadron.currentDaysMissions = missionPlans; 


    //update date and time to the start of the next mission
    Core::Instance().GetApp()->GetCampaignData().CurrentDateTime = Squadron::GetCurrentMission(squadron)->missionPlan.missionDate;
    //add new day news
    if (CampaignLayerPtr)
    {
        std::string weatherReport;

        NewsItem newDayNews;
        newDayNews.newsHeading = "A New Day Has Started";
        std::string NewsString = newDate.ToDateString() + " -- Check for new mission assignments \n";
        newDayNews.newsText = NewsString + "Weather Report: " + Core::Instance().GetApp()->GetCampaignData().CurrentWeather.GetWeatherReport();
        newDayNews.newsImagePath = "data/images/news/NewDay01.jpeg";
        CampaignLayerPtr->ClearNewsItems();
        CampaignLayerPtr->AddNewsItem(newDayNews);
        CampaignLayerPtr->ShowNewsItems();


    }
}

float CampaignManager::GetAggressionFactor(EMissionType type)
{
    switch (type) {
    case EMissionType::INTERCEPT: return 1.0f;
    case EMissionType::ESCORT: return 0.8f;
    case EMissionType::HUNT: return 0.9f;
    case EMissionType::CAP_HIGH:
    case EMissionType::CAP_LOW: return 0.8f;
    case EMissionType::PATROL_GROUND: return 0.3f;
    case EMissionType::BOMBING:
    case EMissionType::STRIKE:
    case EMissionType::GROUND_ATTACK: return 0.3f;
    default: return 0.0f;
    }
}

std::vector<Encounter> CampaignManager::CheckSquadronEncounters(const std::vector<std::pair<Squadron, MissionPlan>>& missions, const Squadron& playerSquadron )
{
    std::vector<Encounter> encounters;
    std::unordered_map<std::string, int> encountersPerSquadron;

    for (size_t i = 0; i < missions.size(); ++i) {
        for (size_t j = i + 1; j < missions.size(); ++j) {
           const auto& [squadA, planA] = missions[i];
           const auto& [squadB, planB] = missions[j];

            // Skip if same side
            if (squadA.side == squadB.side)
                continue;

            float distance = Core::GetShortestDistance(planA.missionArea.worldPosition, planB.missionArea.worldPosition);

            // Max engagement range (in meters), e.g. 25 km
            float maxRange = 30000.0f;
            float minDistance = 10000.0f;
            if (distance > maxRange) continue;

            // Distance scaling
            float distFactor = std::clamp(1.0f - ((distance - minDistance) / maxRange),0.0f,1.0f);

            // Aggression scaling
            float aggressionA = GetAggressionFactor(planA.missionType.type);
            float aggressionB = GetAggressionFactor(planB.missionType.type);

            float typeBonus = (squadA.type == "Fighter" || squadB.type == "Fighter") ? 0.1f : 0.0f;

            float encounterProbability = (std::max(aggressionA,aggressionB) * distFactor) + typeBonus;

            Vec3 encounterPosition = Core::GetRandomPointBetweenWithJitter(planA.missionArea.worldPosition, planB.missionArea.worldPosition);

            // Random roll
            float roll = Core::RandomFloat(0.0f, 1.0f);
            bool willFight = roll < encounterProbability;
            std::vector<std::vector<std::pair<PilotData,SquadronAircraft>>> assignedPilots;
            assignedPilots.push_back(std::vector<std::pair<PilotData, SquadronAircraft>>());
            assignedPilots.push_back(std::vector<std::pair<PilotData, SquadronAircraft>>());
            int index = 0;
            for (const auto& squad : { squadA, squadB })
            {
                std::vector<SquadronAircraft> AvaAircraft;
                for (int i = 0; i < squad.activeAircraft.size(); ++i)
                {
                    if (squad.activeAircraft[i].status == EAIRCRAFT_READY)
                    {
                        AvaAircraft.push_back(squad.activeAircraft[i]);
                    }

                }
                if (squad.id != playerSquadron.id)
                {
                    int numberOfPilots = Core::GetWeightedRandomIntFromRange(3, std::min<int>({ 9, ReadyPilots(squad), ReadyAircraft(squad) }), 0.2, 1, 0.2);
                    MissionRequirements newReq;
                    newReq.numberToSelect = numberOfPilots;
                    newReq.randomizeSelection = true;
                    std::unordered_map<std::string, int> PilotNumbers;
                    std::vector<const PilotData*> assignedPilotsPtr = missionGenerator.GetAvailablePilotsForMission(squad.Pilots, newReq, PilotNumbers);
                    int AircraftIndex = 0;
                    for (const auto& a : assignedPilotsPtr)
                    {
                        std::pair<PilotData, SquadronAircraft> input = { *a , AvaAircraft[AircraftIndex] };
                        assignedPilots[index].push_back(input);
                        AircraftIndex++;
                    }
                }
                else
                {
                    for (int i = 0; i < Squadron::GetCurrentMission(playerSquadron)->AssignedPilots.size(); ++i)
                    {
                        std::pair<PilotData, SquadronAircraft> input = { Squadron::GetCurrentMission(playerSquadron)->AssignedPilots[i], Squadron::GetCurrentMission(playerSquadron)->assignedAircraft[i] };
                        assignedPilots[index].push_back(input);
                    }
                   
                }

                index++;
            }    

            auto itA = encountersPerSquadron.find(squadA.id);
            if (itA != encountersPerSquadron.end()) {
                int encounterCount = itA->second;
                if (encounterCount > m_MaxEncountersPerSquadPerSimulatedMission) continue;
            }
            auto itB = encountersPerSquadron.find(squadA.id);
            if (itB != encountersPerSquadron.end()) {
                int encounterCount = itB->second;
                if (encounterCount > m_MaxEncountersPerSquadPerSimulatedMission) continue;
            }

            encounters.push_back(Encounter{
                &squadA,
                assignedPilots[0],
                &squadB,
                assignedPilots[1],
                encounterProbability,
                distance,
                willFight,
                encounterPosition
                });

            if (willFight)
            {
                encountersPerSquadron[squadA.id]++;
                encountersPerSquadron[squadB.id]++;
            }

        }
    }

    return encounters;
}

void CampaignManager::SimulateAirCombat(const Encounter& encounter, std::vector<EncounterResult>& outResults, CombatState& combatState)
{
    if (!encounter.willEngage) return;
    if (combatState.retreatedSquadrons.find(encounter.squadA->id) != combatState.retreatedSquadrons.end() || combatState.retreatedSquadrons.find(encounter.squadB->id) != combatState.retreatedSquadrons.end()) return;
    std::vector<std::pair<PilotData, SquadronAircraft>> turnOrder;

    // Combine and calculate turn order with random variation
    auto gatherTurnEntries = [&](const std::vector<std::pair<PilotData, SquadronAircraft>>& pilots) {
        for (const auto& pair : pilots) {
            const auto& pilot = pair.first;
            const auto& aircraft = pair.second;

            const AircraftData* acData = GetAircraftByType(aircraft.type);
            if (!acData) continue;

            float baseSpeed = acData->cruisingSpeed;
            float skillModifier = pilot.Score *0.5f;
            float fighterBonus = (std::find(acData->roleCategories.begin(), acData->roleCategories.end(), "FIGHTER") != acData->roleCategories.end()) ? 20.0f : 0.0f;

            // Add a random variation of ±5%
            std::random_device rd;  // Obtain a random seed from hardware
            std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<float> dis(0.95f, 1.05); // Range
            float randomOffset = dis(gen);

            float score = (baseSpeed + skillModifier + fighterBonus)  * randomOffset;

            turnOrder.emplace_back(pair);
            turnOrder.back().first.tempCombatScore = score; // temp score used for sorting
        }
        };

    gatherTurnEntries(encounter.squadAPilots);
    gatherTurnEntries(encounter.squadBPilots);

    std::sort(turnOrder.begin(), turnOrder.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first.tempCombatScore > rhs.first.tempCombatScore;
        });

    std::set<std::string> alreadyTargeted;
    std::unordered_map<std::string, int> extraTurnTracker;
    for (size_t i = 0; i < turnOrder.size(); ++i)
    {
        const auto& attackerPair = turnOrder[i];
        const PilotData& attacker = attackerPair.first;
        const SquadronAircraft& attackerAircraft = attackerPair.second;

        if (alreadyTargeted.find(attacker.PilotName) != alreadyTargeted.end()) continue;

        if (combatState.DownedPilots.find(attacker.PilotName) != combatState.DownedPilots.end()) continue;
        if (combatState.RTBAircraft.find(attacker.PilotName) != combatState.RTBAircraft.end()) continue;

        // Select opposing side
        std::vector<std::pair<PilotData, SquadronAircraft>> opponents = encounter.squadA == nullptr || attacker.SquadronId == encounter.squadA->id
            ? encounter.squadBPilots
            : encounter.squadAPilots;

        // Collect all untargeted opponents
        std::vector<std::pair<PilotData, SquadronAircraft>*> untargeted;

        for (auto& opp : opponents) {
            if (alreadyTargeted.find(opp.first.PilotName) == alreadyTargeted.end()) {
                untargeted.push_back(&opp);
            }
        }


        if (untargeted.empty()) continue; // No available targets

        // Select a random one
        int randomIndex = Core::GetRandomInt(0, static_cast<int>(untargeted.size()) - 1);
        auto* targetPair = untargeted[randomIndex];
        

        const PilotData& target = targetPair->first;
        const SquadronAircraft& targetAircraft = targetPair->second;

        float aircraftQualityDelta = GetAircraftByType(attackerAircraft.type)->goodness - GetAircraftByType(targetAircraft.type)->goodness;

        std::string planeSize = GetAircraftByType(attackerAircraft.type)->planeSize;
        float sizeBonus = 0.0f;
        if (planeSize == "PLANE_SIZE_MEDIUM") sizeBonus = 8.0f;
        if (planeSize == "PLANE_SIZE_LARGE") sizeBonus = 16.0f;

        // Resolve outcome
        float hitChance = std::clamp(attacker.Score * 0.5f, 0.0f, 100.0f) + aircraftQualityDelta + Core::RandomFloat(-20.0f, 20.0f) - sizeBonus;
        float evadeChance = std::clamp(target.Score * 0.5f, 0.0f, 100.0f) + Core::RandomFloat(0.0f, 45.0f) + sizeBonus;

        float finalChance = hitChance - evadeChance;

        EEncounterResult result = EEncounterResult::NONE;
        if (finalChance > 40.0f) {
            float roll = Core::RandomFloat(0.0f, 1.0f);
            if (roll < 0.3f) result = EEncounterResult::SHOTDOWN_KILLED;
            else if (roll < 0.6f) result = EEncounterResult::SHOTDOWN_BAILED;
            else result = EEncounterResult::SHOTDOWN_CRASHLANDING;
                alreadyTargeted.insert(targetPair->first.PilotName);
                combatState.DownedPilots.insert(targetPair->first.PilotName);
        }
        else if (finalChance > 10.0f) {
            float roll = Core::RandomFloat(0.0f, 1.0f);
            if (roll < 0.2f) result = EEncounterResult::DAMAGED_SEVERE;
            else if (roll < 0.6f) result = EEncounterResult::DAMAGED_MEDIUM;
            else result = EEncounterResult::DAMAGED_MINOR;
            alreadyTargeted.insert(targetPair->first.PilotName);
        }

        if (result != EEncounterResult::NONE)
        {
            Vec3 location = Core::GetRandomPointIn2DRadius(encounter.worldLocation, 5000.0f);
            int pilotInjury = GeneratePilotInjury(result);
            int aircraftDamage = GenerateAircraftDamage(result);
            outResults.push_back({ *targetPair, attackerPair, result, location, pilotInjury, aircraftDamage });
        }
        
        std::string key = attacker.PilotName;
        float extraTurnChance = std::clamp(attacker.Score * 0.01f,0.0f,1.0f); // Higher skill = higher chance
        float extraTurnPenalty = (1 - (extraTurnTracker[key] * 0.4f));
        if (Core::RandomFloat(0.0f, 1.0f) < (extraTurnChance * extraTurnPenalty)) {
            turnOrder.push_back(attackerPair);
            if (extraTurnTracker[key] > 0)
            {
                extraTurnTracker[key]++;
            }
            else
            {
                extraTurnTracker[key] = 1;
            }
            
            
        }
        

    }

}


// Get up to 3 mission times, each 2 hours apart
std::vector<std::string> CampaignManager::GetMissionTimes(const std::string& startTime, const std::string& endTime) {
    int start = DateTime::TimeStringToMinutes(startTime);
    int end = (DateTime::TimeStringToMinutes(endTime)) - 1; //one hour before sundown

    // Generate candidate times every 15 mins
    std::vector<int> candidates;
    for (int t = start; t <= end; t += 15)
        candidates.push_back(DateTime::RoundToNearest15(t));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(candidates.begin(), candidates.end(), gen);

    std::vector<int> selected;
    for (int t : candidates) {
        bool valid = true;
        for (int s : selected) {
            if (std::abs(t - s) < 120) {
                valid = false;
                break;
            }
        }
        if (valid) {
            selected.push_back(t);
            if (selected.size() == 3)
                break;
        }
    }

    std::sort(selected.begin(), selected.end());
    std::vector<std::string> result;
    for (int t : selected)
        result.push_back(DateTime::MinutesToTimeString(t));

    return result;
}