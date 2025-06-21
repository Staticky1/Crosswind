#pragma once

#include "Core.h"
#include "Campaign/Mission/MissionType.h"
#include "Core/AircraftData.h"

enum ESquadronAircraftStatus
{
	EAIRCRAFT_READY,
	EAIRCRAFT_MAINTANCE,
	EAIRCRAFT_UPGRADE
};

enum EVictoryType
{
	VICTORY_NONE,
	AIRCRAFT_LIGHT,
	AIRCRAFT_MEDIUM,
	AIRCRAFT_HEAVY,
	AIRCRAFT_PARKED,
	VEHICLE_TRANSPORT,
	VEHICLE_LIGHTARMOR,
	VEHICLE_MEDIUMARMOR,
	VEHICLE_HEAVYARMOR,
	RAIL_LOCOMOTIVE,
	RAIL_RAILCAR,
	RAIL_STATION,
	WEAPON_MG,
	WEAPON_CANNON,
	WEAPON_AAA,
	WEAPON_ROCKETLAUNCHER,
	BUILDING_RESIDENTIAL,
	BUILDING_FACILITY,
	BUILDING_BRIDGE,
	MARINE_LIGHT,
	MARINE_CARGO,
	MARINE_SUBMARINE,
	MARINE_DESTROYER,
};

struct PilotVictory
{
	EVictoryType type = EVictoryType::VICTORY_NONE;
	std::string itemName = "Unknown";
	std::string date = "Unknown";
	std::string location = "Unknown";
};

struct AircraftLoadout
{
	std::string name;
	int gameValue = 0;
};

struct SquadronAircraft
{
	std::string type;
	std::string code; //tail number, etc
	ESquadronAircraftStatus status = EAIRCRAFT_READY;
	std::string skin;
	std::vector<std::string> modifications;
	int maintanceDuration = 0;
	int loadoutIndex = 0;
	std::vector<AircraftLoadout> availableLoadouts;
};

struct Rank {
	std::string id;
	std::string name;
	std::string abbrevation;
	int level = 0;
	int minScoreReq = 0;

	Rank() = default;
};

enum EPilotStatus
{
	EPILOT_NONE = 0,
	EPILOT_READY = 1 << 0,
	EPILOT_ONLEAVE = 1 << 1,
	EPILOT_WOUNDED = 1 << 2,
	EPILOT_CAPTURED = 1 << 3,
	EPILOT_DEAD = 1 << 4,
};

inline EPilotStatus operator|(EPilotStatus a, EPilotStatus b)
{
	return static_cast<EPilotStatus>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool HasStatus(EPilotStatus flags, EPilotStatus test)
{
	return (static_cast<int>(flags) & static_cast<int>(test)) != 0;
}

struct PilotData
{
	bool bIsPlayerPilot = false;

	std::string PilotName;
	std::string SquadronId;
	std::string ServiceId;
	std::string Nationality;
	std::string Background;
	Rank Rank;

	std::shared_ptr<Walnut::Image> IconTexture = nullptr;
	std::string IconPath;

	bool bFemalePilot = false;

	float Score = 0.0f;
	int Missions = 0;

	EPilotStatus PilotStatus = EPILOT_READY;
	int woundDuration = 0;

	std::vector<PilotVictory> Victories;
	float tempCombatScore = 0;

	PilotData() = default;
};

struct SquadronMissionType
{
	EMissionType missionType;
	float missionChance = 0.f;
};

struct SquadronMission
{
	MissionPlan missionPlan;
	std::vector<PilotData> AssignedPilots;
	std::vector<SquadronAircraft> assignedAircraft;
	bool missionComplete = false;
};

struct Squadron {
	std::string side;
	std::string service;
	std::string id;
	std::string name;
	std::string iconPath;
	std::string type;
	float experience = 0.5f;

	std::vector<ValueStartDate> aircraft;
	std::vector<ValueStartDate> locations;

	std::vector<PilotData> Pilots;
	std::vector<SquadronAircraft> activeAircraft;

	std::vector<std::pair<DateTime,std::vector<SquadronMissionType>>> missionTypes;
	std::vector<SquadronMission> currentDaysMissions;

	bool bSquadronInitialized = false;

	Squadron() = default;
public:
	static SquadronMission* GetCurrentMission(Squadron& playerSquadron)
	{
		for (int i = 0; i < playerSquadron.currentDaysMissions.size(); i++)
		{
			if (!playerSquadron.currentDaysMissions[i].missionComplete)
			{
				return &playerSquadron.currentDaysMissions[i];
			}
		}
		return nullptr;
	}
	static const SquadronMission* GetCurrentMission(const Squadron& playerSquadron)
	{
		for (int i = 0; i < playerSquadron.currentDaysMissions.size(); i++)
		{
			if (!playerSquadron.currentDaysMissions[i].missionComplete)
			{
				return &playerSquadron.currentDaysMissions[i];
			}
		}
		return nullptr;
	}

	const int ReadyPilots()
	{
		int count = 0;
		for (const auto& p : Pilots)
		{
			if (p.PilotStatus & EPILOT_READY)
			{
				count++;
			}
		}

		return count;
	}

	const int ReadyAircraft()
	{
		int count = 0;
		for (const auto& p : activeAircraft)
		{
			if (p.status == EAIRCRAFT_READY)
			{
				count++;
			}
		}

		return count;
	}

	PilotData& GetPilotFromNameRank(std::string pilotName, std::string rank)
	{
		PilotData defaultData = PilotData();
		for (int i = 0; i < Pilots.size(); i++)
		{
			if (Pilots[i].PilotName == pilotName && Pilots[i].Rank.id == rank)
			{
				return Pilots[i];
			}
		}

		return defaultData;
	}
};

