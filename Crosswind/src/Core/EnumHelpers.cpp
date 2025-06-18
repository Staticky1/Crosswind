#include "EnumHelpers.h"
#include "PilotData.h"

namespace Crosswind
{
    // ESquadronAircraftStatus specializations
    template<>
    const char* ToString(ESquadronAircraftStatus status) {
        switch (status) {
        case EAIRCRAFT_READY:     return "EAIRCRAFT_READY";
        case EAIRCRAFT_MAINTANCE: return "EAIRCRAFT_MAINTANCE";
        case EAIRCRAFT_UPGRADE:   return "EAIRCRAFT_UPGRADE";
        default: return "Unknown";
        }
    }

    template<>
    ESquadronAircraftStatus FromString<ESquadronAircraftStatus>(const std::string& str) {
        if (str == "EAIRCRAFT_READY") return EAIRCRAFT_READY;
        if (str == "EAIRCRAFT_MAINTANCE") return EAIRCRAFT_MAINTANCE;
        if (str == "EAIRCRAFT_UPGRADE") return EAIRCRAFT_UPGRADE;
        return EAIRCRAFT_READY;
    }

    // EPilotStatus specializations
    template<>
    const char* ToString(EPilotStatus status) {
        switch (status) {
        case EPILOT_READY:     return "EPILOT_READY";
        case EPILOT_ONLEAVE:   return "EPILOT_ONLEAVE";
        case EPILOT_WOUNDED:   return "EPILOT_WOUNDED";
        case EPILOT_CAPTURED:  return "EPILOT_CAPTURED";
        case EPILOT_DEAD:      return "EPILOT_DEAD";
        default: return "Unknown";
        }
    }

    template<>
    EPilotStatus FromString<EPilotStatus>(const std::string& str) {
        if (str == "EPILOT_READY")    return EPILOT_READY;
        if (str == "EPILOT_ONLEAVE")  return EPILOT_ONLEAVE;
        if (str == "EPILOT_WOUNDED")  return EPILOT_WOUNDED;
        if (str == "EPILOT_CAPTURED") return EPILOT_CAPTURED;
        if (str == "EPILOT_DEAD")     return EPILOT_DEAD;
        return EPILOT_READY;
    }

    // EVictoryType specializations
    template<>
    const char* ToString(EVictoryType type) {
        switch (type) {
        case AIRCRAFT_LIGHT:         return "AIRCRAFT_LIGHT";
        case AIRCRAFT_MEDIUM:        return "AIRCRAFT_MEDIUM";
        case AIRCRAFT_HEAVY:         return "AIRCRAFT_HEAVY";
        case AIRCRAFT_PARKED:        return "AIRCRAFT_PARKED";
        case VEHICLE_TRANSPORT:      return "VEHICLE_TRANSPORT";
        case VEHICLE_LIGHTARMOR:     return "VEHICLE_LIGHTARMOR";
        case VEHICLE_MEDIUMARMOR:    return "VEHICLE_MEDIUMARMOR";
        case VEHICLE_HEAVYARMOR:     return "VEHICLE_HEAVYARMOR";
        case RAIL_LOCOMOTIVE:        return "RAIL_LOCOMOTIVE";
        case RAIL_RAILCAR:           return "RAIL_RAILCAR";
        case RAIL_STATION:           return "RAIL_STATION";
        case WEAPON_MG:              return "WEAPON_MG";
        case WEAPON_CANNON:          return "WEAPON_CANNON";
        case WEAPON_AAA:             return "WEAPON_AAA";
        case WEAPON_ROCKETLAUNCHER:  return "WEAPON_ROCKETLAUNCHER";
        case BUILDING_RESIDENTIAL:   return "BUILDING_RESIDENTIAL";
        case BUILDING_FACILITY:      return "BUILDING_FACILITY";
        case BUILDING_BRIDGE:        return "BUILDING_BRIDGE";
        case MARINE_LIGHT:           return "MARINE_LIGHT";
        case MARINE_CARGO:           return "MARINE_CARGO";
        case MARINE_SUBMARINE:       return "MARINE_SUBMARINE";
        case MARINE_DESTROYER:       return "MARINE_DESTROYER";
        default: return "Unknown";
        }
    }

    template<>
    EVictoryType FromString<EVictoryType>(const std::string& str) {
        if (str == "AIRCRAFT_LIGHT")         return AIRCRAFT_LIGHT;
        if (str == "AIRCRAFT_MEDIUM")        return AIRCRAFT_MEDIUM;
        if (str == "AIRCRAFT_HEAVY")         return AIRCRAFT_HEAVY;
        if (str == "AIRCRAFT_PARKED")        return AIRCRAFT_PARKED;
        if (str == "VEHICLE_TRANSPORT")      return VEHICLE_TRANSPORT;
        if (str == "VEHICLE_LIGHTARMOR")     return VEHICLE_LIGHTARMOR;
        if (str == "VEHICLE_MEDIUMARMOR")    return VEHICLE_MEDIUMARMOR;
        if (str == "VEHICLE_HEAVYARMOR")     return VEHICLE_HEAVYARMOR;
        if (str == "RAIL_LOCOMOTIVE")        return RAIL_LOCOMOTIVE;
        if (str == "RAIL_RAILCAR")           return RAIL_RAILCAR;
        if (str == "RAIL_STATION")           return RAIL_STATION;
        if (str == "WEAPON_MG")              return WEAPON_MG;
        if (str == "WEAPON_CANNON")          return WEAPON_CANNON;
        if (str == "WEAPON_AAA")             return WEAPON_AAA;
        if (str == "WEAPON_ROCKETLAUNCHER")  return WEAPON_ROCKETLAUNCHER;
        if (str == "BUILDING_RESIDENTIAL")   return BUILDING_RESIDENTIAL;
        if (str == "BUILDING_FACILITY")      return BUILDING_FACILITY;
        if (str == "BUILDING_BRIDGE")        return BUILDING_BRIDGE;
        if (str == "MARINE_LIGHT")           return MARINE_LIGHT;
        if (str == "MARINE_CARGO")           return MARINE_CARGO;
        if (str == "MARINE_SUBMARINE")       return MARINE_SUBMARINE;
        if (str == "MARINE_DESTROYER")       return MARINE_DESTROYER;

        return AIRCRAFT_LIGHT; // Default fallback
    }
  
    // EMissionType specializations
    template<>
    const char* ToString(EMissionType type) {
        switch (type) {
        case EMissionType::NONE:           return "NONE";
        case EMissionType::INTERCEPT:      return "INTERCEPT";
        case EMissionType::ESCORT:         return "ESCORT";
        case EMissionType::STRIKE:         return "STRIKE";
        case EMissionType::HUNT:           return "HUNT";
        case EMissionType::CAP_HIGH:       return "CAP_HIGH";
        case EMissionType::CAP_LOW:        return "CAP_LOW";
        case EMissionType::GROUND_ATTACK:  return "GROUND_ATTACK";
        case EMissionType::PATROL_GROUND:  return "PATROL_GROUND";
        case EMissionType::BOMBING:        return "BOMBING";
        default:                           return "Unknown";
        }
    }

    template<>
    EMissionType FromString<EMissionType>(const std::string& str) {
        if (str == "NONE")            return EMissionType::NONE;
        if (str == "INTERCEPT")       return EMissionType::INTERCEPT;
        if (str == "ESCORT")          return EMissionType::ESCORT;
        if (str == "STRIKE")          return EMissionType::STRIKE;
        if (str == "HUNT")            return EMissionType::HUNT;
        if (str == "CAP_HIGH")        return EMissionType::CAP_HIGH;
        if (str == "CAP_LOW")         return EMissionType::CAP_LOW;
        if (str == "GROUND_ATTACK")   return EMissionType::GROUND_ATTACK;
        if (str == "PATROL_GROUND")   return EMissionType::PATROL_GROUND;
        if (str == "BOMBING")         return EMissionType::BOMBING;
        return EMissionType::NONE;
    }

    bool IsVictoryTypeInCategory(EVictoryType type, const std::string& categoryPrefix)
    {
        std::string name = ToString<EVictoryType>(type); // Example: "AIRCRAFT_HEAVY"
        return name.rfind(categoryPrefix, 0) == 0;   // Check if prefix matches start of string
    }

    int CountVictoriesByCategory(const PilotData& pilot, const std::string& categoryPrefix)
    {
        int count = 0;
        for (const auto& victory : pilot.Victories)
        {
            if (IsVictoryTypeInCategory(victory.type, categoryPrefix))
                ++count;
        }
        return count;
    }

    std::unordered_map<EVictoryType, int> GetVictoryBreakdown(const PilotData& pilot, const std::string& categoryPrefix, bool includeEmpty)
    {
        std::unordered_map<EVictoryType, int> breakdown;

        // First pass: count actual victories
        for (const auto& victory : pilot.Victories)
        {
            if (IsVictoryTypeInCategory(victory.type, categoryPrefix))
                breakdown[victory.type]++;
        }

        // Optionally add missing types in this category with count 0
        if (includeEmpty)
        {
            for (int i = AIRCRAFT_LIGHT; i <= MARINE_DESTROYER; ++i)
            {
                EVictoryType type = static_cast<EVictoryType>(i);
                if (IsVictoryTypeInCategory(type, categoryPrefix))
                {
                    if (breakdown.find(type) == breakdown.end())
                        breakdown[type] = 0;
                }
            }
        }

        return breakdown;
    }

    std::string GetCategoryName(const std::string& prefix)
    {
        if (prefix == "AIRCRAFT") return "Aircraft";
        if (prefix == "VEHICLE") return "Vehicles";
        if (prefix == "RAIL") return "Rail";
        if (prefix == "WEAPON") return "Weapons";
        if (prefix == "BUILDING") return "Buildings";
        if (prefix == "MARINE") return "Marine";
        return prefix;
    }

}
