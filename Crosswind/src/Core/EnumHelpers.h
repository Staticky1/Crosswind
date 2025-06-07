#pragma once

#include "Core.h"

namespace Crosswind
{
	// Generic template (fallback)
	template<typename T>
	const char* ToString(T);

	template<typename T>
	T FromString(const std::string&);
}

enum ESquadronAircraftStatus;
enum EPilotStatus;
enum EVictoryType;