#include "MissionGroup.h"

void MissionGroup::AddObjectToGroup(std::shared_ptr<MissionObject> object)
{
	GroupedObjects.push_back(object);
}

int MissionGroup::ObjectsInGroup()
{
	return GroupedObjects.size();
}

void MissionGroup::WriteFields(std::ostream& out)
{
	// Write base fields
	out << "  Name = \"" << name << "\";\n";
	out << "  Index = " << index << ";\n";
	out << "  Desc = \"" << description << "\";\n";

	// Write each grouped object
	for (const auto& obj : GroupedObjects)
	{
		std::string text = obj->GetMissionFileText();

		// Indent each line of the object's text by two spaces
		std::istringstream stream(text);
		std::string line;
		while (std::getline(stream, line))
		{
			out << "  " << line << "\n";
		}
	}
}