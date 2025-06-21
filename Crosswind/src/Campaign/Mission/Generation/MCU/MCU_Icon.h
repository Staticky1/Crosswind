#pragma once

#include "MCUObject.h"

class MCU_Icon : public MCUObject
{
public:
	MCU_Icon() : MCUObject(EMissionObjectType::MCU_Icon) {}

	int Enabled = 1;
	int LCName = 0;
	int LCDesc = 0;
	int IconId = 0;
	int RColor = 0;
	int GColor = 255;
	int BColor = 255;
	int LineType = 0;
	std::vector<int> Coalitions;

protected:
	void WriteFields(std::ostream& out) override;
};