#pragma once

#include <iostream>
#include <vector>
#include "imgui.h"
#include "Core.h"
#include "Core/Airfields.h"
#include "PilotData.h"

struct Frontlines {
	std::string side;
	bool bIsPocket = false;
	std::vector<Vec3> points;

	Frontlines() = default;
};

struct Phase {
	std::string name;
	std::string startDate;
	std::string description;
	std::vector<ValueStartDate> frontLinesPaths;

	Phase() = default;
};

struct Service {
	std::string id;
	std::string icon;
	std::string side;
	std::string nationality;
	std::string picture;

	std::vector<Rank> Ranks;

	Service() = default;
};



class Theater 
{
public:
	Theater();

	void LoadTheaterFromFile(const std::string& filePath);
	static Theater LoadTheaterFromId(int TheaterIndex);

	void MergeSquadrons(std::vector<Squadron>& baseSquadrons, const std::vector<Squadron>& loadedSquadrons);

	void LoadSquadronsFromTheater();

	Squadron& GetSquadronsFromID(std::string InputID);

	const Service& GetServiceFromID(std::string InputID);

	void LoadAirfields();

	void LoadFrontLines(std::string date);

	Airfields TheaterAirfields;

	std::string name;
	std::vector<Phase> phases;
	ImVec2 MapTextureSize;
	ImVec2 MapWorldSize;
	std::string MapTexturePath;
	std::vector<Service> Services;
	std::string path;

	std::vector<Frontlines> LoadedFrontlines;

	std::vector<Squadron> AllSquadrons;

	int theaterIndex;

};