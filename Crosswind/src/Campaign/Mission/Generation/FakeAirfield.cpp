#include "FakeAirfield.h"

void FakeAirfield::WriteFields(std::ostream& out)
{
	WorldObject::WriteFields(out);
	out << "  Callsign = " << Callsign << "; \n";
	out << "  Callnum = " << Callnum << ";\n";
	
	if (Chart.size() > 0)
	{
		out << "  Chart\n";
		out << "  {\n";
		for (const auto& p : Chart)
		{
			out << "    Point\n";
			out << "    {\n";
			out << "      Type = " << p.Type << ";\n";
			out << "      X = " << p.X << ";\n";
			out << "      Y = " << p.Y << ";\n";
			out << "    }\n";
		}
		out << "  }\n";
	}

	out << "  ReturnPlanes = " << ReturnPlanes << ";\n";
	out << "  Hydrodrome = " << Hydrodrome << ";\n";
	out << "  RepairFriendlies = " << RepairFriendlies << ";\n";
	out << "  RehealFriendlies = " << RehealFriendlies << ";\n";
	out << "  RearmFriendlies = " << RearmFriendlies << ";\n";
	out << "  RefuelFriendlies = " << RefuelFriendlies << ";\n";
	out << "  RepairTime = " << RepairTime << ";\n";
	out << "  RehealTime = " << RehealTime << ";\n";
	out << "  RearmTime = " << RearmTime << ";\n";
	out << "  RefuelTime = " << RearmTime << ";\n";
	out << "  MaintenanceRadius = " << RearmTime << ";\n";
}
