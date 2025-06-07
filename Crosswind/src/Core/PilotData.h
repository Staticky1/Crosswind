#pragma once

#include "Core.h"

enum ESquadronAircraftStatus
{
	EAIRCRAFT_READY,
	EAIRCRAFT_MAINTANCE,
	EAIRCRAFT_UPGRADE
};

enum EVictoryType
{
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
	EVictoryType type;
	std::string itemName = "Unknown";
	std::string date = "Unknown";
	std::string location = "Unknown";
};

struct SquadronAircraft
{
	std::string type;
	std::string code; //tail number, etc
	ESquadronAircraftStatus status = EAIRCRAFT_READY;
	std::string skin;
	std::vector<std::string> modifications;
	int maintanceDuration = 0;
};

struct Rank {
	std::string id;
	std::string name;
	std::string abbrevation;
	int level = 0;
	int minScoreReq = 0;
};

enum EPilotStatus
{
	EPILOT_READY,
	EPILOT_ONLEAVE,
	EPILOT_WOUNDED,
	EPILOT_CAPTURED,
	EPILOT_DEAD,
};

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

	bool bSquadronInitialized = false;

	Squadron() = default;
};

