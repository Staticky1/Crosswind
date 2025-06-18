#pragma once

#include "Core.h"
#include "Core/PilotData.h"
#include "UI/MainMenu/CampaignSetupData.h"
#include "Core/AircraftData.h"
#include "Mission/MissionPlanGenerator.h"
#include "Core/CampaignData.h"
#include <unordered_set>

//simulated encounter info
struct Encounter {
	const Squadron* squadA;
	std::vector<std::pair<PilotData, SquadronAircraft>> squadAPilots;
	const Squadron* squadB;
	std::vector<std::pair<PilotData, SquadronAircraft>> squadBPilots;
	float probability; // chance of engagement
	float distance;
	bool willEngage;
	Vec3 worldLocation;
};

enum class EEncounterResult
{
	SHOTDOWN_KILLED,
	SHOTDOWN_BAILED,
	SHOTDOWN_CRASHLANDING,
	DAMAGED_SEVERE,
	DAMAGED_MEDIUM,
	DAMAGED_MINOR,
	NONE,
};

struct EncounterResult {
	std::pair<PilotData, SquadronAircraft> targetPilot;
	std::pair<PilotData, SquadronAircraft> attackingPilot;
	EEncounterResult result;
	Vec3 worldLocation;
	int pilotInjury = 0;
	int aircraftMaintance = 0;
};

struct CombatState {
	std::unordered_set<std::string> DownedPilots;
	std::unordered_set<std::string> RTBAircraft;
	std::unordered_set<std::string> retreatedSquadrons;
};

//Class that handles the logic of a campaign

class CampaignLayer;

class CampaignManager
{
public:
	static CampaignManager& Instance();

	void SetCampaignLayer(CampaignLayer* campaignLayer);

	bool CreateNewCampaignSave(const CampaignSetupData& setupData, const PilotData& playerPilot);
	const std::string& GetCurrentCampaignName() const;

	bool SaveCampaignData(CampaignData& campaignData, int selectedPilotIndex);

	bool LoadAircraftDataFromXML(const std::string& filename);
	const std::unordered_map<std::string, AircraftData>& GetAircraftList();
	const AircraftData* GetAircraftByType(const std::string& id);

	bool LoadCampaignSaveFile(const std::string& FileName);

	bool LoadCampaignSave(const std::string& xmlFilePath, std::vector<Squadron>& outSquadrons, std::string& outDate, std::string& outTime, int& campaignIndex, std::vector<PilotData>& playerPilots, WeatherInfo& info);

	void BackupExistingSave(const std::string& saveFilePath);

	void RequestNewDay(DateTime& currentDate, Theater* theater, std::vector<PilotData>& playerPilots);

	void SimulateMission(DateTime& missionDate, Theater* theater, SquadronMission& mission, Squadron& playerSquadron, std::vector<EncounterResult>& results);

	void UpdateMissionData(Squadron& playerSquadron);

	std::vector<Squadron> GetNearbySquadrons(const DateTime& currentTime, Theater* theater, Vec3 worldLocation, float maxRangeMod = 0.5f);

	static int ReadyPilots(const Squadron& squadron)
	{
		int count = 0;
		for (const auto& p : squadron.Pilots)
		{
			if (p.PilotStatus & EPILOT_READY)
			{
				count++;
			}
		}

		return count;
	}

	static int ReadyAircraft(const Squadron& squadron)
	{
		int count = 0;
		for (const auto& p : squadron.activeAircraft)
		{
			if (p.status == EAIRCRAFT_READY)
			{
				count++;
			}
		}

		return count;
	}

	CampaignLayer* GetCampaignLayer() const;

	void UpdateCampaignData(CampaignData& campaignData, std::vector<EncounterResult>& results);

	int GeneratePilotInjury(EEncounterResult resultType);

	int GenerateAircraftDamage(EEncounterResult resultType);

private:
	CampaignManager() = default;

	std::unordered_map<std::string,AircraftData> m_AircraftList;

	std::string m_CurrentCampaignName;

	std::vector<MissionPlan> DaysMission;

	std::string BuildCampaignSavePath(const std::string& campaignName) const;

	void InitSquadrons(CampaignSetupData& setupData, const PilotData& playerPilot);

	void CreateNewPilot(const Service& Service, const Squadron& Squadron, const DateTime& CurrentDate, PilotData& pilotData);
	int SelectCappedRankIndex(const std::vector<Rank>& ranks, float experienceModifier, std::unordered_map<int, int>& currentCounts, const std::unordered_map<int, int>& maxAllowedPerLevel);
	void AssignMandatoryHighRanks(const std::vector<Rank>& ranks, int numHighRanksNeeded, std::unordered_map<int, int>& currentCounts, const std::unordered_map<int, int>& maxAllowedPerLevel, std::vector<int>& preassignedIndices);
	EVictoryType GetRandomVictoryType(std::vector<EVictoryType> possibleTypes);

	std::vector<std::string> GetMissionTimes(const std::string& startTime, const std::string& endTime);

	MissionPlanGenerator missionGenerator;

	CampaignLayer* CampaignLayerPtr = nullptr;

	void StartNewDay(DateTime newDate, Theater* theater, Squadron& squadron);

	float GetAggressionFactor(EMissionType type);

	std::vector<Encounter> CheckSquadronEncounters(const std::vector<std::pair<Squadron, MissionPlan>>& missions, const Squadron& playerSquadron);

	void SimulateAirCombat(const Encounter& encounter, std::vector<EncounterResult>& outResults, CombatState& combatState);

	std::vector<std::pair<PilotData, SquadronAircraft>> GetInitiativeOrder(
		const std::vector<std::pair<PilotData, SquadronAircraft>>& aPilots,
		const std::vector<std::pair<PilotData, SquadronAircraft>>& bPilots);

	//config variables
	int m_MaxEncountersPerSquadPerSimulatedMission = 2;

	float m_InjuryChanceOnBail = 0.4f;
	float m_ModerateInjuryOnBail = 0.3f;
	float m_MajorInjuryOnBail = 0.1f;

	float m_InjuryChanceOnCrashLand = 0.5f;
	float m_ModerateInjuryOnCrashLand = 0.4f;
	float m_MajorInjuryOnCrashLand = 0.2f;

	float m_InjuryChanceOnAircraftDamage = 0.4f; //scales down based on the level of aircraft damage,

	int m_MinorInjuryMaxDays = 5;
	int m_ModerateInjuryMaxDays = 15;
	int m_MajorInjuryMaxDays = 30;

	int m_MinorMaintanceMaxDays = 3;
	int m_ModerateMaintanceMaxDays = 6;
	int m_MajorMaintanceMaxDays = 12;
};