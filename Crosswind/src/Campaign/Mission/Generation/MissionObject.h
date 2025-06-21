#pragma once

#include "Core.h"
#include "Campaign/Mission/MissionBuildData.h"

class MissionObject
{

public:
	MissionObject(EMissionObjectType ObjectType);
	
	int index = 0; //unique object index
	std::string name;
	std::string description;
	Vec3 Pos;
	Vec3 Ori;

	virtual std::string GetMissionFileText();
protected:
	EMissionObjectType objectType;

	virtual void WriteFields(std::ostream& out);
};