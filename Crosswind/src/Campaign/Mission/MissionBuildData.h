#pragma once

#include "Core.h"

struct MissionWaypoint
{
	std::string name;
	Vec3 WorldLocation;
	int index = 0;
	std::vector<int> targets;
	std::vector<int> objects;
	int triggerArea = 200;
	int speed = 200;
	int Priority = 1;//chance of ignoring waypoint to engage enemy 0 - low, 1 - medium, 2 - high
	bool canBeEdited = false;
};

struct MissionTR_Begin
{
	Vec3 WorldLocation;
	int index = 0;
	std::vector<int> targets;
	std::vector<int> objects;
	bool enabled = true;
};

struct MissionTimer
{
	Vec3 WorldLocation;
	int index = 0;
	std::vector<int> targets;
	std::vector<int> objects;
	float time = 0; //in seconds
	int random = 100; //percentage chance of trigger
};

enum ECountry
{
	Neutral = 0,
	USSR = 101,
	GreatBritain = 102,
	UnitedStates = 103,
	Germany = 201,
	Italy = 202,
	Japan = 203,
	France_WW1 = 301,
	GreatBritain_WW1 = 302,
	UnitedStates_WW1 = 303,
	Belgium_WW1 = 304,
	Russia_WW1 = 305,
	Germany_WW1 = 401,
	AustriaHungary_WW1 = 402,
	RedTeam = 901,
	BlueTeam = 902,
	YellowTeam = 903,
	GreenTeam = 904,
};

enum EAILevel
{
	PLAYER = 0,
	LOW = 1,
	NORMAL = 2,
	HIGH = 3,
	ACE = 4,
};

enum ECallsign
{
	Stork = 1,
	Gannet = 2,
	Raven = 3,
	Rook = 4,
	Thrush = 5,
	Crane = 6,
	Finch = 7,
	Oriole = 8,
	Canary = 9,
	Swan = 10,
	Kittiwake = 11,
	Eagle = 12,
	Pelican = 13,
	Swift = 14,
	Redshank = 15,
	Duck = 16,
	Pheasant = 17,
	Seagull = 18,
	Brambling = 19,
	Hawk = 20
};

enum EAirfieldCallsign
{
	Acacia = 21,
	Beech = 22,
	Elm = 23,
	Oak = 24,
	Hornbeam = 25,
	Spruce = 26,
	Jasmine = 27,
	Willow = 28,
	Maple = 29,
	Linden = 30,
	Magnolia = 31,
	Alder = 32,
	Fir = 33,
	Rowan = 34,
	Pine = 35,
	Thuja = 36,
	Pistachio = 37,
	Mulberry = 39,
	Eucalyptus = 40,
	Ash = 41,
};

enum EEventType
{
	OnPilotKilled = 0,
	OnPilotWounded = 1,
	OnPlaneCrashed = 2,
	OnPlaneCriticalDamage = 3,
	OnPlaneDestroyed = 4,
	OnPlaneLanded = 5,
	OnPlaneTookOff = 6,
	OnPlaneBingoFuel = 7,
	OnPlaneBingoMainMG = 8,
	OnPlaneBingoBombs = 9,
	OnPlaneBingoTurrets = 10,
	OnPlaneGunnerskilled = 11,
	OnDamaged = 12,
	OnKilled = 13,
	OnMovedTo = 14,
	OnPlaneBingoCargo = 15,
};

enum class EMissionObjectType
{
	None,
	Options,
	Group,
	Plane,
	Block,
	Bridge,
	Vehicle,
	Airfield,
	//mcu
	MCU_Waypoint,
	MCU_Timer,
	MCU_Icon,
	MCU_TR_MissionBegin,
	MCU_CMD_TakeOff,
	MCU_CMD_Land,
	MCU_TR_InfluenceArea,
};

namespace Crosswind
{
	inline const char* GetTypeString(EMissionObjectType type)
	{
		switch (type)
		{
		case EMissionObjectType::Options:             return "Options";
		case EMissionObjectType::Group:               return "Group";
		case EMissionObjectType::Plane:               return "Plane";
		case EMissionObjectType::Block:               return "Block";
		case EMissionObjectType::Bridge:			  return "Bridge";
		case EMissionObjectType::Vehicle:             return "Vehicle";
		case EMissionObjectType::Airfield:            return "Airfield";
		case EMissionObjectType::MCU_Waypoint:        return "MCU_Waypoint";
		case EMissionObjectType::MCU_Timer:           return "MCU_Timer";
		case EMissionObjectType::MCU_Icon:           return "MCU_Icon";
		case EMissionObjectType::MCU_TR_MissionBegin: return "MCU_TR_MissionBegin";
		case EMissionObjectType::MCU_CMD_TakeOff:     return "MCU_CMD_TakeOff";
		case EMissionObjectType::MCU_CMD_Land:        return "MCU_CMD_Land";
		case EMissionObjectType::MCU_TR_InfluenceArea: return "MCU_TR_InfluenceArea";
		default:                                      return "Unknown";
		}
	}
}

struct OnEvent
{
	EEventType type;
	int target = 0;
};

struct MissionEntity
{
	int index = 0;
	std::string name;
	std::vector<int> targets;
	std::vector<int> objects;
	Vec3 worldLocation;
	Vec3 orientation;
	bool enabled = true;
	int missionObjectId = 0;
	std::vector<OnEvent> onEvents;
};

struct MissionCommand
{
	int index = 0;
	std::string name;
	std::vector<int> targets;
	std::vector<int> objects;
	Vec3 worldLocation;
	Vec3 orientation;
	std::unordered_map<std::string, int> extraArguments;
	std::unordered_map<std::string, std::string> extraStringArguments;
};

struct MissionPlane
{
	std::string Name;
	int index = 0;
	int linkTrId = 0;
	Vec3 worldLocation;
	Vec3 Orientation;
	std::string script;
	std::string model;
	ECountry country;
	std::string skin;
	EAILevel aiLevel;
	bool coopStart = false;
	bool engageable = true;
	bool limitAmmo = true;
	int startInAir = 1; //0 - in air, 1 - On runway, 2 - parked
	ECallsign callsign;
	int callnum = 0;
	int time = 60; //route time in minutes
	int damageReport = 50;
	bool damageThreshold = true;
	bool aiRTBDecision = true;
	bool deleteAfterDeath = true;
	int payloadId = 0;
	int WMMask = 1;
	int spotter = -1; //if above 0 = range
	float fuel = 1.0f; //0.0 - 1.0
	std::string tCode;
	std::string tCodeColor;
};

struct ChartPoint {
	int Type = 0; //0 = parking, 1 = taxi, 2 = runway
	float X = 0.0f;
	float Y = 0.0f;
};

