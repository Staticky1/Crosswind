#pragma once

#include <tinyxml2/tinyxml2.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "Core.h"

struct Orientation {
    float xOri, yOri, zOri;
};

struct ParkingLocation {
    std::string name;
    Vec3 position;
    Orientation orientation;
};

struct RunwayData {
    Vec3 startPos;
    Vec3 endPos;
    std::vector<Vec3> taxiToStart;
    std::vector<Vec3> taxiFromEnd;
    ParkingLocation parking;
};

struct Airfield {
    std::string name;
    Vec3 position;
    Orientation orientation;
    RunwayData runways;
};

class Airfields 
{
public:
    std::unordered_map<std::string, Airfield> LoadAirfields(const std::string& filePath);

    std::unordered_map<std::string, Airfield> LoadedAirfields;

private:
    Vec3 ParseVec3(tinyxml2::XMLElement* elem);
    Orientation ParseOrientation(tinyxml2::XMLElement* elem);
    Airfield ParseAirfield(tinyxml2::XMLElement* elem);
};