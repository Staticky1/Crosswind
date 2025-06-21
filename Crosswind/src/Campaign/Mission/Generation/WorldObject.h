#pragma once

#include "MissionObject.h"

class WorldObject : public MissionObject
{
public:
	WorldObject(EMissionObjectType type = EMissionObjectType::None) : MissionObject(type) {}

	int LinkTrId = 0;
	std::string Model;
	std::string Script;
	int Country = 0;
	int Durability = 10000;
	int DamageReport = 50;
	int DamageThreshold = 1;
	int DeleteAfterDeath = 1;
	std::vector<std::pair<int, int>> Damaged;

protected:
	void WriteFields(std::ostream& out) override;
};