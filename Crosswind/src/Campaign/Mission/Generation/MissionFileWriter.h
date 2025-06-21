#pragma once

#include "Core.h"
#include "Core/CampaignData.h"
#include "MissionObject.h"
#include "MissionGroup.h"
#include "WorldObject.h"
#include <map>

struct MissionBounds {
	Vec3 min; // Lower corner of the box
	Vec3 max; // Upper corner of the box
};

class MissionFileWriter
{
public:
	MissionFileWriter(CampaignData& inCampaignData, int& SelectedPlayerPilot, SquadronMission& inMission);

	template <typename T, typename... Args>
	std::shared_ptr<T> CreateObject(Args&&... args);

	void SetupMissionOptions();

	void WriteMissionFile();
	bool WriteLCTable();

	MissionBounds CalculateMissionBounds(const Vec3& airfieldPos, const std::vector<MissionWaypoint>& waypoints, float padding = 10000.0f);

	void ParseStaticObjectXml(const std::string& filePath);
	std::vector<std::shared_ptr<MissionObject>> ParseXmlElement(tinyxml2::XMLElement* elem, std::shared_ptr<MissionGroup> currentGroup, const MissionBounds& missionBounds);

	void AddAirfields(const Airfields* theaterAirfields);

private:
	CampaignData& m_CampaignData;
	int& m_SelectedPlayerPilotIndex;
	SquadronMission& m_Mission;
 
	std::unordered_map<std::shared_ptr<MissionObject>, int> objectIndexMap;

	MissionBounds m_MissionBounds;

	// Stores and owns all mission objects
	std::vector<std::shared_ptr<MissionObject>> missionObjects;

	std::vector<std::pair<int, std::string>> LCTable;

	int AddLCTableItem(std::string text);

	std::string ConvertTimeToFullFormat(const std::string& timeStr);

	void LoadWorldObjectAttributes(tinyxml2::XMLElement* elem, WorldObject* object);

	void LoadGroupAttributes(tinyxml2::XMLElement* elem, MissionGroup* currentGroup);

	bool XmlLocationWithinBounds(tinyxml2::XMLElement* elem);

	bool IsWithinMissionBounds(const Vec3& position);

};

template <typename T, typename... Args>
std::shared_ptr<T> MissionFileWriter::CreateObject(Args&&... args)
{
	static_assert(std::is_base_of<MissionObject, T>::value, "T must inherit from MissionObject");

	auto obj = std::make_shared<T>(std::forward<Args>(args)...);

	obj->index = static_cast<int>(objectIndexMap.size() + 3);
	objectIndexMap[obj] = obj->index;

	return obj;
}