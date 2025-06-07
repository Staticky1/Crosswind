#pragma once

#include <string>
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



#define CONFIG Core::Instance().GetConfig()
#define LOC    Core::Instance().GetLocalizationManager()

//simple struct with a start date and value param
struct ValueStartDate
{
    std::string startDate;  // in "yyyy-mm-dd"
    std::string value;
};

struct Vec3 {
    float x, y, z;
};

class LocalizationManager;
class ConfigManager;
class CrosswindApp;

class Core {
public:
    static Core& Instance();

    void Initialize();
    void Shutdown();

    static std::time_t ParseDate(const std::string& dateStr);
    static std::optional<std::string> GetActiveValue(const std::vector<ValueStartDate>& values, const std::string& currentDateStr);

    static bool HasDatePassed(const std::string& inputDate, const std::string& referenceDate);

    LocalizationManager& GetLocalizationManager();

    void SetApp(CrosswindApp* inApp);
    CrosswindApp* GetApp();

    static bool ApplicationIsVisible();

    static ImVec2 WorldToMap(Vec3 WorldPos, ImVec2 WorldSize, ImVec2 MapSize);
    static Vec3 MapToWorld(ImVec2 WorldPos, ImVec2 WorldSize, ImVec2 MapSize);

    template<typename T>
        static const T* GetActiveEntryRef(
            const std::vector<T>& entries,
            const std::string& currentDateStr,
            std::function<std::string(const T&)> getStartDate)
    {
        std::time_t current = ParseDate(currentDateStr);
        const T* result = nullptr;

        for (const auto& entry : entries) {
            std::time_t entryTime = ParseDate(getStartDate(entry));
            if (entryTime <= current) {
                if (!result || ParseDate(getStartDate(*result)) < entryTime) {
                    result = &entry;
                }
            }
        }

        return result; // nullptr if nothing matches
    }
    
    //Random

    static int GetWeightedRandomIndex(const std::vector<double>& weights);
    static bool GetWeightedRandomBool(double chanceTrue = 0.3);
    static int GetWeightedRandomIntFromRange(int min, int max, double weightBottom, double weightMiddle, double weightTop);

    ConfigManager& GetConfig();
    // Add accessors for other global subsystems here...

    static ImVec2 Lerp(ImVec2 a, ImVec2 b, float t);

private:
    Core();
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

    // LocalizationManager is a singleton, so just store a reference or pointer
    LocalizationManager* localizationManager = nullptr;
    ConfigManager config;

    CrosswindApp* m_App = nullptr;

};