#pragma once

#include "Core.h"

namespace Crosswind
{
	std::vector<std::string> LoadNamesFromFile(const std::string& filePath);
	std::string GetName(std::string nationality, bool bFemalePilot);

	std::string GetRandomName(const std::vector<std::string>& list);

	std::string GetBackground(std::string service, std::string name, bool newBackground, bool bIsFemale = false);

	std::string GetPilotImage(std::string nationality, bool bIsFemale = false);
}