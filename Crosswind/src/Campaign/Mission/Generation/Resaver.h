#pragma once

#include "Core.h"
#include <filesystem>

class Resaver
{
public:

	bool RunResaver(const std::filesystem::path& resaverDir, const std::filesystem::path& missionPath);
};