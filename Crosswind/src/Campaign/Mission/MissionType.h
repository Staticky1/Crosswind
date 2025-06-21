#pragma once

#include "Core.h"
#include "MissionBuildData.h"

enum class EMissionType {
    NONE,
    INTERCEPT,
    ESCORT,
    STRIKE,
    HUNT,
    CAP_HIGH,
    CAP_LOW,    // Combat Air Patrol
    GROUND_ATTACK,
    PATROL_GROUND,
    BOMBING
};

struct MissionTemplate {
    EMissionType type = EMissionType::NONE;
    std::string name;
    std::string description;
};

struct MissionTargetArea {
    std::string id; //stores the closest map location
    Vec3 worldPosition = Vec3();
    std::string control; // "Axis" or "Allies"
    std::string type;    // "Frontline", "Depot", "Bridge", etc.
};

struct MissionPlan {
    DateTime missionDate;
    MissionTemplate missionType;
    MissionTargetArea missionArea;
    std::vector<MissionWaypoint> missionWaypoints;
};

struct MissionRequirements
{
    int numberToSelect = 1;
    bool prioritizeByScore = false;
    bool randomizeSelection = false;
};