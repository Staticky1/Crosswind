#include "MissionPlanGenerator.h"
#include "Core/EnumHelpers.h"
#include "Campaign/CampaignManager.h"
#include <set>

MissionPlan MissionPlanGenerator::CreateMissionPlan(Theater* theater, const Squadron& squadron, const DateTime& currentDateTime, float maxRangeMod)
{
    //Select Mission Type
    const auto* selectedMissionTypes = Core::GetActiveEntryRef<std::pair<DateTime, std::vector<SquadronMissionType>>>(
        squadron.missionTypes,
        currentDateTime,
        [](const std::pair<DateTime, std::vector<SquadronMissionType>>& e) { return e.first; }
    );

    std::vector<float> weightsIndex;
    std::vector<SquadronMissionType> missionTypes = selectedMissionTypes->second;

    for (int i = 0; i < missionTypes.size(); ++i)
    {
       float weight = (missionTypes[i].missionChance / 100);
       weightsIndex.push_back(weight);
    }
    int selectedIndex = Core::Instance().GetWeightedRandomIndex(weightsIndex);

    EMissionType selectedType = missionTypes[selectedIndex].missionType;

    selectedType = EMissionType::CAP_LOW; //always select cap for now

    //Setup base mission values
    const auto* AircraftValue = Core::GetActiveEntryRef<ValueStartDate>(
        squadron.aircraft,
        currentDateTime,
        [](const ValueStartDate& e) { return e.startDate; }
    );
    const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
        squadron.locations,
        currentDateTime,
        [](const ValueStartDate& e) { return e.startDate; }
    );

     const AircraftData* aircraftData = CampaignManager::Instance().GetAircraftByType(AircraftValue->value);
     float maxWaypointRange = aircraftData->range * 500.0f;
     
     const Airfield selectedAirfield = theater->GetAirfields()->LoadedAirfields.at(AirfieldValue->value);
     
     std::string enemySide = squadron.side == "Allies" ? "Axis" : "Allies";
     //mission locaion will vary depending on mission type

     std::vector<MajorBattle> onGoingBattles = theater->GetOngoingBattles(currentDateTime);

     switch (selectedType)
     {
     case EMissionType::CAP_LOW:
     {
         //find front line segments that are within max range

         std::vector<Vec3> candidates;
         std::vector<MajorBattle> battleCandidates;

         std::vector<Frontlines> allFrontlines = theater->GetFrontlines(currentDateTime);

         //Get all enemy frontlines that are within max mission range
         for (const auto& frontlines : allFrontlines)
         {
             if (frontlines.side == enemySide)
             {
                 for (const auto& segment : frontlines.points) {
                     float distToBase = Core::GetShortestDistance(selectedAirfield.position, segment);
                     if (distToBase <= (maxWaypointRange * maxRangeMod)) {
                         candidates.push_back(segment);
                     }
                 }
             }
         }

         for (const auto& battle : onGoingBattles)
         {
             float distToBase = Core::GetShortestDistance(selectedAirfield.position, battle.worldLocation);
             if (distToBase <= (maxWaypointRange * maxRangeMod))
             {
                 battleCandidates.push_back(battle);
             }
         }

         bool bIsBattlePatrol = false;
         MajorBattle selectedBattle;
         float battlePatrolSizeWeightFactor = 0.7f;
         //decide if frontline patrol or battle patrol
         if (battleCandidates.size() != 0)
         {
             std::vector<MajorBattle> pickedBattle;
             for (const auto& battle : battleCandidates)
             {
                 float distanceFactor = 0;
                 float distanceOffset = Core::GetShortestDistance(selectedAirfield.position, battle.worldLocation) - 10000;
                 distanceFactor = std::clamp(distanceFactor / maxWaypointRange, 0.0f, 1.0f);
                 float sizeFactor = std::clamp(battle.radius / 12000, 0.0f, 1.0f) * battlePatrolSizeWeightFactor;
                 if (Core::GetWeightedRandomBool(distanceFactor * sizeFactor))
                 {
                     pickedBattle.push_back(battle); //battle has been selected as a possible target for the patrol.
                 }

                 if (pickedBattle.size() > 0) //if any battles where picked
                 {
                     selectedBattle = Core::GetRandomVectorElement<MajorBattle>(pickedBattle);
                     MissionPlan NewPlan;
                     NewPlan.missionType.type = selectedType;
                     NewPlan.missionArea.id = GetClosestLocation(theater, candidates[selectedIndex]).id;
                     NewPlan.missionArea.type = "Ground Battle";
                     NewPlan.missionArea.worldPosition = selectedBattle.worldLocation;
                     NewPlan.missionDate = currentDateTime;
                     GenerateMissionNameAndDescription(NewPlan);
                     return NewPlan;
                     break;
                 }
             }
         }

         //Has not picked a battle zone, so will pick frontlines

         if (candidates.size() > 0)
         {
             std::vector<float> scores;
             
             //weight frontlines of nearby battles
             float battleProximityFactor = 0.5f;
             float battlePromimityMax = 25000.0f;
             float battlePromimityMin = 5000.0f;
             if (battleCandidates.size() > 0)
             {
                 std::vector<Vec3> battleLocations;
                 for (const auto& battle : battleCandidates)
                 {
                     battleLocations.push_back(battle.worldLocation);
                 }
                 scores = ScorePointsByProximity(candidates, battleLocations, battlePromimityMin, battlePromimityMax);
                 for (int i = 0; i < scores.size(); ++i)
                 {
                     float v = scores[i];
                     scores[i] = (v * battleProximityFactor) + 1;
                 }
                 
             }

             if (scores.size() == 0)
             {
                 for (int i = 0; i < candidates.size(); ++i)
                 {
                     scores.push_back(1);
                 }
             }
                 
             int selectedIndex = Core::GetWeightedRandomIndex(scores);

             MissionPlan NewPlan;
             NewPlan.missionType.type = selectedType;
             NewPlan.missionArea.id = GetClosestLocation(theater, candidates[selectedIndex]).id;
             NewPlan.missionArea.type = "Frontline";
             NewPlan.missionArea.worldPosition = candidates[selectedIndex];
             NewPlan.missionDate = currentDateTime;
             GenerateMissionNameAndDescription(NewPlan);
             return NewPlan;
             break;
         }

     }
     }

    return MissionPlan();
}

std::vector<float> MissionPlanGenerator::ScorePointsByProximity(
    const std::vector<Vec3>& points,
    const std::vector<Vec3>& battleLocations,
    float minDistance,
    float maxDistance)
{
    std::vector<float> scores;
    scores.reserve(points.size());

    for (const Vec3& point : points)
    {
        float closestDistance = std::numeric_limits<float>::max();

        for (const Vec3& battle : battleLocations)
        {
            float dist = Core::GetShortestDistance(point, battle);
            if (dist < closestDistance)
                closestDistance = dist;
        }

        float score = RemapDistanceToScore(closestDistance, minDistance, maxDistance);
        scores.push_back(score);
    }

    return scores;
}

float MissionPlanGenerator::RemapDistanceToScore(float distance, float minDistance, float maxDistance)
{
    if (distance <= minDistance)
        return 1.0f;
    if (distance >= maxDistance)
        return 0.0f;

    float normalized = (distance - minDistance) / (maxDistance - minDistance);
    return 1.0f - normalized; // Linear falloff
}

const MapLocation MissionPlanGenerator::GetClosestLocation(Theater* theater, Vec3 worldPos)
{
    std::vector<MapLocation>& Locations = theater->GetMapLocations();

    float shortestDistance = 9999999.0f;
    MapLocation closestLocation;
    if (Locations.size() > 0)
    {
        for (const auto& loc : Locations)
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

std::vector<const PilotData*> MissionPlanGenerator::GetAvailablePilotsForMission(
    const std::vector<PilotData>& pilots,
    const MissionRequirements& requirements,
    const std::unordered_map<std::string, int>& missionCounts )// PilotID -> mission count
{
    std::vector<const PilotData*> available;

    // Step 1: Filter by readiness
    for (const auto& pilot : pilots)
    {
        if (pilot.PilotStatus & EPILOT_READY)
            available.push_back(&pilot);
    }

    if (available.empty())
        return {};

    // Step 2: Sort primarily by mission count (ascending), then by rank (descending)
    std::sort(available.begin(), available.end(),
        [&](const PilotData* a, const PilotData* b) {
            int missionsA = missionCounts.count(a->PilotName) ? missionCounts.at(a->PilotName) : 0;
            int missionsB = missionCounts.count(b->PilotName) ? missionCounts.at(b->PilotName) : 0;

            if (missionsA != missionsB)
                return missionsA < missionsB; // Fewer missions preferred

            return a->Rank.level > b->Rank.level; // If same, higher rank first
        });

    // Step 3: Pick at least one high-ranking pilot (from top half)
    std::vector<const PilotData*> selected;

    size_t topHalfSize = available.size() / 2;
    if (topHalfSize == 0) topHalfSize = 1; // fallback if only one

    // Always include a high-ranking pilot if possible
    int leaderIndex = Core::GetRandomInt(0, topHalfSize);

    selected.push_back(available[leaderIndex]); // random high-ranked pilot
    available.erase(available.begin() + leaderIndex);

    // Step 5: Optionally shuffle (after ensuring rank priority)
    if (requirements.randomizeSelection)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::stable_sort(available.begin(), available.end(),
            [&](const PilotData* a, const PilotData* b) {
                int missionsA = missionCounts.count(a->PilotName) ? missionCounts.at(a->PilotName) : 0;
                int missionsB = missionCounts.count(b->PilotName) ? missionCounts.at(b->PilotName) : 0;
                return missionsA < missionsB;
            });

        // Keep the first (high-rank) fixed, shuffle the rest
        std::shuffle(available.begin() + 1, available.end(), gen);
    }
    // Step 4: Fill remaining slots
    for (size_t i = 1; i < available.size() && selected.size() < requirements.numberToSelect; ++i)
    {
        selected.push_back(available[i]);
    }

    return selected;
}

std::vector<const SquadronAircraft*> MissionPlanGenerator::GetAvailableAircraftForMission(const std::vector<SquadronAircraft>& aircraft, const MissionRequirements& requirements)
{
    std::vector<const SquadronAircraft*> returnAircraft;
    int count = 0;
    for (int i = 0; i < aircraft.size(); ++i)
    {
        if (aircraft[i].status == EAIRCRAFT_READY)
        {
            returnAircraft.push_back(&aircraft[i]);
            count++;

            if (count == requirements.numberToSelect)
            {
                return returnAircraft;
            }
        }
    }

    return returnAircraft;
}

void MissionPlanGenerator::GenerateMissionNameAndDescription(MissionPlan& plan)
{
    std::string dateStr = plan.missionDate.ToTimeString(); // You should already have this method
    const std::string& targetName = plan.missionArea.id;
    const std::string& targetType = plan.missionArea.type;
    const std::string& control = plan.missionArea.control;

    std::string name;
    std::string description;

    switch (plan.missionType.type) {
    case EMissionType::INTERCEPT:
        name = "Intercept Mission";
        description = "An enemy air raid is expected near " + targetName + " on " + dateStr +
            ". Intercept incoming aircraft before they reach their target.";
        break;
    case EMissionType::ESCORT:
        name = "Escort Mission";
        description = "Provide escort for friendly aircraft en route to " + targetName +
            ". Ensure their safe passage through hostile airspace.";
        break;
    case EMissionType::STRIKE:
        name = "Strike Mission";
        description = "A tactical strike has been ordered on the enemy-controlled " + targetType +
            " near " + targetName + ". Deliver precision firepower and return safely.";
        break;
    case EMissionType::HUNT:
        name = "Fighter Sweep";
        description = "Conduct a fighter sweep in the vicinity of " + targetName +
            " to engage and eliminate enemy aircraft in the area.";
        break;
    case EMissionType::CAP_HIGH:
        name = "High Altitude CAP";
        description = "Establish a high altitude combat air patrol over " + targetType + " near " + targetName +
            ". Focus on targeting enemy aircraft operating at altitude.";
        break;
    case EMissionType::CAP_LOW:
        name = "Low Altitude CAP";
        description = "Conduct low altitude air patrol over " + targetName +
            ", focusing on protecting ground forces from air threats.";
        break;
    case EMissionType::GROUND_ATTACK:
        name = "Ground Attack";
        description = "Attack enemy ground units or infrastructure in the area of " + targetName +
            ". Target is under " + control + " control.";
        break;
    case EMissionType::PATROL_GROUND:
        name = "Recon Patrol";
        description = "Patrol the area near " + targetName + " to gather intelligence on enemy ground movements.";
        break;
    case EMissionType::BOMBING:
        name = "Bombing Mission";
        description = "Bomb enemy positions at the " + targetType + " located at " + targetName +
            ". Target is under " + control + " control.";
        break;
    default:
        name = "Unknown Mission";
        description = "No details available for this mission type.";
        break;
    }

    // Assign the generated name and description
    plan.missionType.name = name;
    plan.missionType.description = description;
}

void MissionPlanGenerator::UpdateMissionPlan(const Squadron& squadronData, SquadronMission& mission)
{
    // Step 1: Build available pools
    std::vector<PilotData> availablePilots;
    std::vector<SquadronAircraft> availableAircraft;

    for (const auto& pilot : squadronData.Pilots)
    {
        if (pilot.PilotStatus & EPILOT_READY)
            availablePilots.push_back(pilot);
    }

    for (const auto& aircraft : squadronData.activeAircraft)
    {
        if (aircraft.status == EAIRCRAFT_READY)
            availableAircraft.push_back(aircraft);
    }

    // Step 2: Track used IDs
    std::set<std::string> usedPilotIds;
    std::set<std::string> usedAircraftIds;

    std::vector<PilotData> updatedPilots;
    std::vector<SquadronAircraft> updatedAircraft;

    size_t count = std::min(mission.AssignedPilots.size(), mission.assignedAircraft.size());

    for (size_t i = 0; i < count; ++i)
    {
        const auto& pilot = mission.AssignedPilots[i];
        const auto& aircraft = mission.assignedAircraft[i];

        bool pilotReady = (pilot.PilotStatus & EPILOT_READY) && !usedPilotIds.count(pilot.PilotName);
        bool aircraftReady = aircraft.status == EAIRCRAFT_READY && !usedAircraftIds.count(aircraft.code);

        PilotData finalPilot = pilot;
        SquadronAircraft finalAircraft = aircraft;

        bool pilotReplaced = false;
        bool aircraftReplaced = false;

        // Replace pilot if needed
        if (!pilotReady)
        {
            pilotReplaced = true;
            bool found = false;
            for (const auto& p : availablePilots)
            {
                if (!usedPilotIds.count(p.PilotName))
                {
                    finalPilot = p;
                    found = true;
                    break;
                }
            }
            if (!found) continue; // Cannot replace pilot, skip pair
        }

        // Replace aircraft if needed
        if (!aircraftReady)
        {
            aircraftReplaced = true;
            bool found = false;
            for (const auto& a : availableAircraft)
            {
                if (!usedAircraftIds.count(a.code))
                {
                    finalAircraft = a;
                    found = true;
                    break;
                }
            }
            if (!found) continue; // Cannot replace aircraft, skip pair
        }

        // Now we have valid pilot/aircraft
        updatedPilots.push_back(finalPilot);
        updatedAircraft.push_back(finalAircraft);
        usedPilotIds.insert(finalPilot.PilotName);
        usedAircraftIds.insert(finalAircraft.code);
    }

    // Step 3: Commit results
    mission.AssignedPilots = std::move(updatedPilots);
    mission.assignedAircraft = std::move(updatedAircraft);
}

