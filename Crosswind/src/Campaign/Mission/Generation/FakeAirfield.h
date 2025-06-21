#pragma once

#include "WorldObject.h"

class FakeAirfield : public WorldObject
{
public:
	FakeAirfield() : WorldObject(EMissionObjectType::Airfield) {}

	int Callsign = EAirfieldCallsign::Acacia;
	int Callnum = 0;
	std::vector<ChartPoint> Chart;
	int ReturnPlanes = 0;
	int Hydrodrome = 0;
	int RepairFriendlies = 0;
	int RehealFriendlies = 0;
	int RearmFriendlies = 0;
	int RefuelFriendlies = 0;
	int RepairTime = 0;
	int RehealTime = 0;
	int RearmTime = 0;
	int RefuelTime = 0;
	int MaintenanceRadius = 1000;

protected:
	void WriteFields(std::ostream& out) override;
};