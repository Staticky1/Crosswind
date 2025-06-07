#include "NewPilot.h"
#include <fstream>
#include <random>
#include <filesystem>


std::vector<std::string> Crosswind::LoadNamesFromFile(const std::string& filePath)
{
	std::vector<std::string> names;
	std::ifstream file(filePath);
	std::string line;

	while (std::getline(file, line)) {
		if (!line.empty())
			names.push_back(line);
	}

	return names;
}

std::string Crosswind::GetName(std::string nationality, bool bFemalePilot)
{

	std::string isFemale = bFemalePilot ? "Female" : "";
	std::string PilotFirstNamesPath = "data/names/FirstNames" + nationality + isFemale + ".txt";
	std::string PilotLastNamesPath = "data/names/LastNames" + nationality + ".txt";

	std::vector<std::string> FirstNames = LoadNamesFromFile(PilotFirstNamesPath);
	std::vector<std::string> LastNames = LoadNamesFromFile(PilotLastNamesPath);

	return GetRandomName(FirstNames) + " " + GetRandomName(LastNames);

}

std::string Crosswind::GetRandomName(const std::vector<std::string>& list)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	if (list.empty()) return "Unknown";

	std::uniform_int_distribution<> distr(0, static_cast<int>(list.size()) - 1);
	return list[distr(gen)];
}

std::string Crosswind::GetBackground(std::string service, std::string name, bool newBackground, bool bIsFemale)
{
	int maxBackgrounds = 0;
	int randomBackgroundIndex = 0;
	if (service == "VVS") {
		maxBackgrounds = 6;
	}
	else if (service == "Normandie")
	{
		maxBackgrounds = 4;
	}
	else if (service == "Luftwaffe")
	{
		maxBackgrounds = 4;
	}
	else if (service == "Regia")
	{
		maxBackgrounds = 4;
	}

	if (newBackground)
		randomBackgroundIndex = rand() % maxBackgrounds;

	std::string BackgroundKey = service + "_PilotBackground" + std::to_string(randomBackgroundIndex);
	std::unordered_map<std::string, std::string> nameMap = {
		{"name", name}
	};

	return LOCAL_FORMAT(BackgroundKey, bIsFemale, nameMap);
}

std::string Crosswind::GetPilotImage(std::string picture, bool bIsFemale)
{
	std::vector<std::string> imageList;
	int SelectedImageIndex = 0;

	std::string basePath = "data/images/pilotPictures/" + picture;
	std::string prefix = picture;

	if (bIsFemale)
	{
		basePath = basePath + "/female";
		prefix = prefix + "Female";
	}

	int index = 1;
	while (true)
	{
		// Format the filename (e.g., Russian01.png)
		std::stringstream ss;
		ss << basePath << "/" << prefix << std::setfill('0') << std::setw(2) << index << ".jpg";
		std::string filePath = ss.str();
		if (!std::filesystem::exists(filePath))
			break;
		imageList.push_back(filePath);
		index++;
	}

	// Random selection
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, static_cast<int>(imageList.size()) - 1);
	SelectedImageIndex = dis(gen);

	return imageList[SelectedImageIndex];
}
