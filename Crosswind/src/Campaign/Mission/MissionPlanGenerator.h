#pragma once

#include "Core.h"
#include "MissionType.h"
#include "Core/Theater.h"
#include "Core/CampaignData.h"
#include "Campaign/Mission/MissionBuildData.h"

class MissionPlanGenerator
{
public:
	//generates the parameters for a squadrons mission
	MissionPlan CreateMissionPlan(Theater* theater, const Squadron& squadron, const DateTime& currentDateTime, const WeatherInfo& currentWeather, float maxRangeMod = 0.5f);

	//std::vector<Vec3> GenerateBattlePatrolWaypoints(MajorBattle battle);

	std::vector<const PilotData*> GetAvailablePilotsForMission(const std::vector<PilotData>& pilots, const MissionRequirements& requirements, const std::unordered_map<std::string, int>& missionCounts);
	std::vector<const SquadronAircraft*> GetAvailableAircraftForMission(const std::vector<SquadronAircraft>& aircraft, const MissionRequirements& requirements);

	static void GenerateMissionNameAndDescription(MissionPlan& plan);

	//
	void UpdateMissionPlan(const Squadron& squadronData, SquadronMission& mission);

	std::vector<MissionWaypoint> GenerateCAPFrontlineWaypoints(
		const Airfield& airfield,
		const Vec3& missionArea,
		const std::vector<Vec3>& frontlinePoints,
		float patrolStepDistance = 8000.0f,
		float crossDistance = 10000.0f,
		float maxTotalDistance = 20000.0f,
		float patrolAltitude = 1000,
		float patrolSpeed = 300
	);

private:

	std::vector<float>	ScorePointsByProximity(
		const std::vector<Vec3>& points,
		const std::vector<Vec3>& battleLocations,
		float minDistance,
		float maxDistance);

	float RemapDistanceToScore(float distance, float minDistance, float maxDistance);

	const MapLocation GetClosestLocation(Theater* theater, Vec3 worldPos);
};