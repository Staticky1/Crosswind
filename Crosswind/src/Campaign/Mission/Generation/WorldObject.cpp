#include "WorldObject.h"

void WorldObject::WriteFields(std::ostream& out)
{
	MissionObject::WriteFields(out);
	out << "  LinkTrId = " << LinkTrId << "; \n";
	out << "  Model = \"" << Model << "\";\n";
	out << "  Script = \"" << Model << "\";\n";
	out << "  Country = " << Country << ";\n";
	out << "  Durability = " << Durability << ";\n";
	out << "  DamageReport = " << DamageReport << ";\n";
	out << "  DamageThreshold = " << DamageThreshold << ";\n";
	out << "  DeleteAfterDeath = " << DeleteAfterDeath << ";\n";
	if (Damaged.size() > 0)
	{
		out << "  Damaged" << "\n";
		out << "  {" << "\n";
		for (int i = 0; i < Damaged.size(); ++i)
		{
			out << "    " << Damaged[i].first << " = " << Damaged[i].second << ";\n";
		}
		out << "  }" << "\n";
	}
}
