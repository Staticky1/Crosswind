#include "Airfields.h"
#include <iostream>

using namespace tinyxml2;

std::unordered_map<std::string, Airfield> Airfields::LoadAirfields(const std::string& filePath)
{
    std::unordered_map<std::string, Airfield> airfields;

    XMLDocument doc;
    if (doc.LoadFile(filePath.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load Airfields XML at " << filePath << "\n";
        return airfields;
    }

    XMLElement* root = doc.FirstChildElement("root");
    for (XMLElement* airfieldElem = root->FirstChildElement(); airfieldElem; airfieldElem = airfieldElem->NextSiblingElement()) {
        std::string key = airfieldElem->Name();
        Airfield af = ParseAirfield(airfieldElem);
        airfields[key] = af;
    }

    LoadedAirfields = airfields;
    return airfields;
}

Vec3 Airfields::ParseVec3(tinyxml2::XMLElement* elem)
{
    Vec3 v;
    elem->FirstChildElement("xPos")->QueryFloatText(&v.x);
    elem->FirstChildElement("yPos")->QueryFloatText(&v.y);
    elem->FirstChildElement("zPos")->QueryFloatText(&v.z);
    return v;
}

Orientation Airfields::ParseOrientation(tinyxml2::XMLElement* elem)
{
    Orientation o;
    elem->FirstChildElement("xOri")->QueryFloatText(&o.xOri);
    elem->FirstChildElement("yOri")->QueryFloatText(&o.yOri);
    elem->FirstChildElement("zOri")->QueryFloatText(&o.zOri);
    return o;
}

Airfield Airfields::ParseAirfield(tinyxml2::XMLElement* elem)
{
    Airfield af;
    af.name = elem->FirstChildElement("name")->GetText();
    af.position = ParseVec3(elem->FirstChildElement("position"));
    af.orientation = ParseOrientation(elem->FirstChildElement("orientation"));

    XMLElement* runways = elem->FirstChildElement("runways");
    af.runways.startPos = ParseVec3(runways->FirstChildElement("startPos"));
    af.runways.endPos = ParseVec3(runways->FirstChildElement("endPos"));

    XMLElement* parking = runways->FirstChildElement("parkingLocation");
    af.runways.parking.name = parking->FirstChildElement("name")->GetText() ? parking->FirstChildElement("name")->GetText() : "";
    af.runways.parking.position = ParseVec3(parking->FirstChildElement("position"));
    af.runways.parking.orientation = ParseOrientation(parking->FirstChildElement("orientation"));

    for (XMLElement* e = runways->FirstChildElement("taxiToStart"); e; e = e->NextSiblingElement("taxiToStart")) {
        af.runways.taxiToStart.push_back(ParseVec3(e));
    }

    for (XMLElement* e = runways->FirstChildElement("taxiFromEnd"); e; e = e->NextSiblingElement("taxiFromEnd")) {
        af.runways.taxiFromEnd.push_back(ParseVec3(e));
    }

    return af;
}
