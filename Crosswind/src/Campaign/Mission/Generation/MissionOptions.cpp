#include "MissionOptions.h"

void MissionOptions::WriteFields(std::ostream& out)
{
	//MissionObject::WriteFields(out);
    out << "  LCName = " << LCName << ";\n";
    out << "  LCDesc = " << LCDesc << ";\n";
    out << "  LCAuthor = " << LCAuthor << ";\n";
    out << "  PlayerConfig = \"" << PlayerConfig << "\";\n";
    out << "  Time = " << Time << ";\n";
    out << "  Date = " << Date << ";\n";
    out << "  HMap = \"" << HMap << "\";\n";
    out << "  Textures = \"" << Textures << "\";\n";
    out << "  Forests = \"" << Forests << "\";\n";
    out << "  Layers = \"" << Layers << "\";\n";
    out << "  GuiMap = \"" << GuiMap << "\";\n";
    out << "  SeasonPrefix = \"" << SeasonPrefix << "\";\n";
    out << "  MissionType = " << MissionType << ";\n";
    out << "  AqmId = " << AqmId << ";\n";
    out << "  CloudLevel = " << CloudLevel << ";\n";
    out << "  CloudHeight = " << CloudHeight << ";\n";
    out << "  PrecLevel = " << PrecLevel << ";\n";
    out << "  PrecType = " << PrecType << ";\n";
    out << "  CloudConfig = \"" << CloudConfig << "\";\n";
    out << "  SeaState = " << SeaState << ";\n";
    out << "  Turbulence = " << Turbulence << ";\n";
    out << "  TempPressLevel = " << TempPressLevel << ";\n";
    out << "  Temperature = " << Temperature << ";\n";
    out << "  Pressure = " << Pressure << ";\n";
    out << "  Haze = " << Haze << ";\n";
    out << "  LayerFog = " << LayerFog << ";\n";

    // WindLayers
    out << "  WindLayers\n";
    out << "  {\n";
    if (!WindLayers.empty()) {
        for (size_t i = 0; i < WindLayers.size(); ++i) {
            float alt = 0;
            float dir = WindLayers[i].first;
            float spd = WindLayers[i].second;
            switch (i) {
            case 0: alt = 0; break;
            case 1: alt = 500; break;
            case 2: alt = 1000; break;
            case 3: alt = 2000; break;
            case 4: alt = 5000; break;
            default: alt = 0; break;
            }
            out << "    " << alt << " :     " << dir << " :     " << spd << ";\n";
        }
    }
    out << "  }\n";

    // Countries
    out << "  Countries\n";
    out << "  {\n";
    for (const auto& [countryId, teamId] : Countries) {
        out << "    " << countryId << " : " << teamId << ";\n";
    }
    out << "  }\n";

}
