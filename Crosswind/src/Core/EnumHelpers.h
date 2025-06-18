#pragma once

#include "Core.h"
#include "PilotData.h"

namespace Crosswind
{
	// Generic template (fallback)
	template<typename T>
	const char* ToString(T);

	template<typename T>
	T FromString(const std::string&);

	bool IsVictoryTypeInCategory(EVictoryType type, const std::string& categoryPrefix);
	int CountVictoriesByCategory(const PilotData& pilot, const std::string& categoryPrefix);
	std::unordered_map<EVictoryType, int> GetVictoryBreakdown(const PilotData& pilot, const std::string& categoryPrefix, bool includeEmpty = false);
	std::string GetCategoryName(const std::string& prefix);

}

enum ESquadronAircraftStatus;
enum EPilotStatus;
enum EVictoryType;