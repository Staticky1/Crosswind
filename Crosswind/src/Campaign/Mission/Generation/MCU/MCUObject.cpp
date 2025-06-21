#include "MCUObject.h"

void MCUObject::AddTarget(MissionObject* missionObject)
{
	if (missionObject)
	{
		Targets.push_back(missionObject->index);
	}
}

void MCUObject::AddObject(MissionObject* missionObject)
{
	if (missionObject)
	{
		Objects.push_back(missionObject->index);
	}
}

void MCUObject::WriteFields(std::ostream& out)
{
	WorldObject::WriteFields(out);
	out << "  Targets = [";
	for (int i = 0; i < Targets.size(); ++i)
	{
		out << Targets[i];
		if (i != Targets.size() - 1)
		{
			out << ",";
		}
	}
	out << "];\n";

	out << "  Objects = [";
	for (int i = 0; i < Objects.size(); ++i)
	{
		out << Objects[i];
		if (i != Objects.size() - 1)
		{
			out << ",";
		}
	}
	out << "];\n";


}
