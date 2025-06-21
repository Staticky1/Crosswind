#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <optional>
#include <algorithm>
#include <memory>
#include <functional>
#include "Config/ConfigManager.h"
#include <imgui.h>
#include "Core/TextureGarbageCollector.h"
#include "Localization/LocalizationManager.h"
#include "Core/DateTime.h"
#include "imSpinner/imspinner.h"

//MissionResaver.exe -d "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data" -f "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data\Missions\Crosswind\Low Altitude CAP_1942_07_22_17_15.Mission"


/*
"E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\bin\resaver\MissionResaver.exe"  -t   -d "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data"  -f "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data\Missions\PWCG\Moscow 1941-10-09.mission"

Succeeded creating binary mission file for: "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\bin\resaver\MissionResaver.exe"  -t   -d "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data"  -f "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data\Missions\PWCG\Moscow 1941-10-09.mission"
*/
//MissionResaver.exe -t -d "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad/data" -f "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data\Missions\Crosswind\Low Altitude CAP_1942_07_22_17_15.mission"
// "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\bin\resaver\MissionResaver.exe"  -t   -d "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad/data"  -f "E:\SteamLibrary\steamapps\common\IL-2 Sturmovik Battle of Stalingrad\data\Missions\Crosswind\Low Altitude CAP_1942_07_22_17_15.mission"

constexpr auto CROSS_VERSION = 0.0;
#define CONFIG Core::Instance().GetConfig()
#define LOC    Core::Instance().GetLocalizationManager()

#define PLAYERCOLOUR  ImVec4(1.0f, 0.85f, 0.0f, 1.0f)

//simple struct with a start date and value param
struct ValueStartDate
{
    DateTime startDate;  // in "yyyy-mm-dd"
    std::string value;
};

struct Vec3 {
    float x, y, z;

    Vec3(float InX = 0, float InY = 0, float InZ = 0)
        :x(InX)
        , y(InY)
        , z(InZ)
    {}

    // Addition
    Vec3 operator+(const Vec3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    // Subtraction
    Vec3 operator-(const Vec3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    // Scalar multiplication (Vec3 * float)
    Vec3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    // Scalar multiplication (float * Vec3)
    friend Vec3 operator*(float scalar, const Vec3& v) {
        return v * scalar;
    }

    // In-place addition
    Vec3& operator+=(const Vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    // In-place subtraction
    Vec3& operator-=(const Vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    // In-place scalar multiplication
    Vec3& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

};

class LocalizationManager;
class ConfigManager;
class CrosswindApp;

class Core {
public:
    static Core& Instance();

    void Initialize();
    void Shutdown();

    static std::string ReformatDate(const std::string& isoDate);

    static std::time_t ParseDate(const std::string& dateStr);
    static std::optional<std::string> GetActiveValue(const std::vector<ValueStartDate>& values, const DateTime& currentDateStr);

    static bool HasDatePassed(const DateTime& inputDate, const DateTime& referenceDate);

    LocalizationManager& GetLocalizationManager();

    void SetApp(CrosswindApp* inApp);
    CrosswindApp* GetApp();

    static bool ApplicationIsVisible();

    static ImVec2 WorldToMap(Vec3 WorldPos, ImVec2 WorldSize, ImVec2 MapSize);
    static Vec3 MapToWorld(ImVec2 WorldPos, ImVec2 WorldSize, ImVec2 MapSize);

    static std::string GetWeightLabel(int value, int min, int max);

    template<typename T>
    static const T* GetActiveEntryRef(
        const std::vector<T>& entries,
        const DateTime& currentDateTime,
        const std::function<DateTime(const T&)>& getStartDateTime)
    {
        const T* result = nullptr;

        for (const auto& entry : entries)
        {
            DateTime entryTime = getStartDateTime(entry);
           // std::cout << "Comparing: " << entryTime.ToFullString()
             //   << " <= " << currentDateTime.ToFullString() << "\n";

            if (entryTime <= currentDateTime)
            {
                if (!result || getStartDateTime(*result) < entryTime)
                {
                    result = &entry;
                   // std::cout << " --> Selected\n";
                }
            }
        }

        return result;
    }
    
    //Random

    static int GetWeightedRandomIndex(const std::vector<float>& weights);
    static int GetWeightedRandomIndex(const std::vector<double>& weights);
    static bool GetWeightedRandomBool(double chanceTrue = 0.3);
    static int GetWeightedRandomIntFromRange(int min, int max, double weightBottom, double weightMiddle, double weightTop);

    static int GetRandomInt(int min, int max);

    template<typename T>
    static T GetRandomVectorElement(const std::vector<T>& vec)
    {
        if (vec.empty())
            throw std::runtime_error("Cannot get random element from an empty vector.");

        int index = rand() % vec.size();
        return vec[index];
    }

    static float RandomFloat(float min = 0.0f, float max = 1.0f);
    static Vec3 GetRandomPointBetween(const Vec3& a, const Vec3& b);
    static Vec3 GetRandomPointBetweenWithJitter(const Vec3& a, const Vec3& b, float maxOffset = 5000.0f);
    static Vec3 GetRandomPointIn2DRadius(const Vec3& center, float radius);

    ConfigManager& GetConfig();
    // Add accessors for other global subsystems here...

    static ImVec2 Lerp(ImVec2 a, ImVec2 b, float t);
    static float Cross2D(const Vec3& a, const Vec3& b);
    static Vec3 Normalize(const Vec3& v);
    static Vec3 Normalize2D(const Vec3& v);
    static Vec3 Perpendicular2D(const Vec3& v);
    static float Length2D(const Vec3& v);

    static constexpr float DegToRad(float degrees) {
        return degrees * 3.14159265f / 180.0f;
    }

    static Vec3 RotateXZ(const Vec3& position, float degrees);

    // --- Distance between two ImVec2 points (2D) ---
    static inline float GetShortestDistance(const ImVec2& a, const ImVec2& b)
    {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // --- Distance between two Vec3 points (2D) ---
    static inline float GetShortestDistance(const Vec3& a, const Vec3& b)
    {
        float dx = b.z - a.z;
        float dy = b.x - a.x;
        return std::sqrt(dx * dx + dy * dy);
    }

    static float DistanceToSegment2D(const Vec3& p, const Vec3& a, const Vec3& b);
    static std::pair<int, Vec3> FindClosestPointOnPolyline2D(const Vec3& point, const std::vector<Vec3>& polyline);



    static float FlipDirection180(float degrees);
    static std::string GetCardinalDirection(float degrees);

    // Returns >0 if 'point' is to the left of the segment from 'a' to 'b', <0 if right, 0 if on the line
    static float GetPointSideOfSegment(const Vec3& a, const Vec3& b, const Vec3& point);

    //adds together any number of strings with spaces between
    static std::string CompileNewsStrings(std::vector<std::string>& inStrings);
    static std::string FloatToString(float value, int decimals);
    static bool StringContains(const std::string& text, const std::string& substring);

    std::filesystem::path GetGamePath();

private:
    Core();
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

    // LocalizationManager is a singleton, so just store a reference or pointer
    LocalizationManager* localizationManager = nullptr;
    ConfigManager config;

    CrosswindApp* m_App = nullptr;

};