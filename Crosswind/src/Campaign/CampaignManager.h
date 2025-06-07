#pragma once

#include "Core.h"
#include "Core/PilotData.h"
#include "UI/MainMenu/CampaignSetupData.h"
#include "Core/AircraftData.h"

//Class that handles the logic of a campaign

class CampaignManager
{
public:
	static CampaignManager& Instance();

	bool CreateNewCampaignSave(const CampaignSetupData& setupData, const PilotData& playerPilot);
	const std::string& GetCurrentCampaignName() const;

	bool LoadAircraftDataFromXML(const std::string& filename);
	const std::vector<AircraftData>& GetAircraftList() const;
	const AircraftData* GetAircraftByType(const std::string& id) const;

	bool LoadCampaignSaveFile(const std::string& FileName);

	bool LoadCampaignSave(const std::string& xmlFilePath, std::vector<Squadron>& outSquadrons, std::string& outDate, std::string& outTime, int& campaignIndex);

private:
	CampaignManager() = default;

	std::vector<AircraftData> m_AircraftList;

	std::string m_CurrentCampaignName;

	std::string BuildCampaignSavePath(const std::string& campaignName) const;

	void InitSquadrons(CampaignSetupData& setupData, const PilotData& playerPilot);

	void CreateNewPilot(const Service& Service, const Squadron& Squadron, std::string CurrentDate, PilotData& pilotData);
	int SelectCappedRankIndex(const std::vector<Rank>& ranks, float experienceModifier, std::unordered_map<int, int>& currentCounts, const std::unordered_map<int, int>& maxAllowedPerLevel);
	void AssignMandatoryHighRanks(const std::vector<Rank>& ranks, int numHighRanksNeeded, std::unordered_map<int, int>& currentCounts, const std::unordered_map<int, int>& maxAllowedPerLevel, std::vector<int>& preassignedIndices);
	EVictoryType GetRandomVictoryType(std::vector<EVictoryType> possibleTypes);



};