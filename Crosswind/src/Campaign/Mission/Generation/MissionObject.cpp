#include "MissionObject.h"


MissionObject::MissionObject(EMissionObjectType ObjectType)
{
	objectType = ObjectType;
}

std::string MissionObject::GetMissionFileText()
{
    std::ostringstream oss;
    oss << Crosswind::GetTypeString(objectType) << "\n{\n";
    WriteFields(oss);
    oss << "}\n";
    return oss.str();
}

void MissionObject::WriteFields(std::ostream& out)
{
    out << "  Name = \"" << name << "\";\n";
    out << "  Index = " << index << ";\n";
    out << "  Desc = \"" << description << "\";\n";
    out << "  XPos = " << Core::FloatToString(Pos.x, 3) << ";\n";
    out << "  YPos = " << Core::FloatToString(Pos.y, 3) << ";\n";
    out << "  ZPos = " << Core::FloatToString(Pos.z, 3) << ";\n";
    out << "  XOri = " << Core::FloatToString(Ori.x, 2) << ";\n";
    out << "  YOri = " << Core::FloatToString(Ori.y, 2) << ";\n";
    out << "  ZOri = " << Core::FloatToString(Ori.z, 2) << ";\n";
}
