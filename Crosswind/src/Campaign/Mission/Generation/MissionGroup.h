#pragma once

#include "MissionObject.h"

class MissionGroup : public MissionObject
{
public:
	MissionGroup() : MissionObject(EMissionObjectType::Group) {}

	void AddObjectToGroup(std::shared_ptr<MissionObject> object);

	int ObjectsInGroup();

protected:
	void WriteFields(std::ostream& out) override;

	std::vector<std::shared_ptr<MissionObject>> GroupedObjects;
};