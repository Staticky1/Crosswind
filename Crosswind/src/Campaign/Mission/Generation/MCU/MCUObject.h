#pragma once

#include "Campaign/Mission/Generation/WorldObject.h"

class MCUObject : public WorldObject
{
public:
	MCUObject(EMissionObjectType type) : WorldObject(type) {}

	std::vector<int> Targets;
	std::vector<int> Objects;

	void AddTarget(MissionObject* missionObject);
	void AddObject(MissionObject* missionObject);

protected:
	void WriteFields(std::ostream& out) override;
};