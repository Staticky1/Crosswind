#pragma once

#include "Core.h"

struct Modification {
    std::string name;
    std::optional<std::string> startDate; // Format: "YYYY-MM-DD"
    std::optional<std::string> endDate;
};

struct AircraftData {
    std::string type;
    std::string archType;
    std::string side;

    std::vector<std::string> primaryUsedBy; // e.g. ["GERMANY", "ITALY"]

    std::vector<Modification> stockModifications;

    std::string displayName;
    std::string script;
    std::string model;
    std::string desc;

    int cruisingSpeed;
    int climbOutRate;
    int goodness;
    int range;

    bool isFlyable;
    bool isNovice;

    std::string planeSize; // Enum-style string: e.g. "PLANE_SIZE_SMALL"

    std::vector<std::string> roleCategories; // e.g. ["FIGHTER", "ATTACK"]

    std::string introduction;   // Format: "YYYY-MM-DD"
    std::string endProduction;  // Format: "YYYY-MM-DD"
    std::string withdrawal;     // Format: "YYYY-MM-DD"

    std::string tacCodeType; //["NUMBER", "LETTER"]

};