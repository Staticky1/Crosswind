#include "Core.h"
#include <filesystem>
#include <iostream>
#include <random>
#include <GLFW/glfw3.h>
#include "Localization/LocalizationManager.h"
#include "Config/ConfigManager.h"
#include "Walnut/Application.h"

Core& Core::Instance() {
    static Core instance;
    return instance;
}

Core::Core() {
    // Constructor is private: no external instantiation
}

void Core::Initialize() 
{
    std::string configPath = "config.ini";

    if (!std::filesystem::exists(configPath)) {
        std::cout << "No config found. Creating default config.\n";
        config.Set("General.Language", "en");
        config.Save(configPath);
    }
    else {
        config.Load(configPath);
    }


    localizationManager = &LocalizationManager::Instance();

    std::string lang = config.Get("General.Language", "en");
    localizationManager->LoadFromFile("data/localization.xml");
    localizationManager->SetLanguage(lang);

    // Initialize other subsystems...
}

LocalizationManager& Core::GetLocalizationManager() {
    return *localizationManager;
}

void Core::SetApp(CrosswindApp* inApp)
{
    m_App = inApp;
}

CrosswindApp* Core::GetApp()
{
    return m_App;
}

bool Core::ApplicationIsVisible()
{
    int width, height;
   
    glfwGetFramebufferSize(Walnut::Application::Get().GetWindowHandle(), &width, &height);
    bool isMinimized = (width == 0 || height == 0);
    return !isMinimized;
}

ImVec2 Core::WorldToMap(Vec3 WorldPos, ImVec2 WorldSize, ImVec2 MapSize)
{
    float xRatio = WorldPos.z / WorldSize.x;
    float yRatio = WorldPos.x / WorldSize.y;

    // Invert Y so that 0 is at the top
    float textureX = xRatio * MapSize.x;
    float textureY = (1.0f - yRatio) * MapSize.y;

    return ImVec2(textureX, textureY);
}

Vec3 Core::MapToWorld(ImVec2 MapPos, ImVec2 WorldSize, ImVec2 MapSize)
{
    float xRatio = MapPos.x / MapSize.x;
    float yRatio = MapPos.y / MapSize.y;

    float worldX = xRatio * WorldSize.x;
    float worldY = (1.0f - yRatio) * WorldSize.y; // flip Y

    return Vec3(worldY,0.0f, worldX);
}



int Core::GetWeightedRandomIndex(const std::vector<double>& weights)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return dist(gen);
}

bool Core::GetWeightedRandomBool(double chanceTrue)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::bernoulli_distribution dist(chanceTrue);
    return dist(gen);
}

int Core::GetWeightedRandomIntFromRange(int min, int max, double weightBottom, double weightMiddle, double weightTop)
{
     if (max < min)
        throw std::invalid_argument("Invalid range");

    static std::random_device rd;
    static std::mt19937 gen(rd());

    int count = max - min + 1;
    std::vector<double> weights(count);

    int midIndex = count / 2;

    for (int i = 0; i < count; ++i) {
        double t = static_cast<double>(i) / (count - 1);
        if (t < 0.5)
            weights[i] = weightBottom + (weightMiddle - weightBottom) * (t * 2);
        else
            weights[i] = weightMiddle + (weightTop - weightMiddle) * ((t - 0.5) * 2);
    }

    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return min + dist(gen);

}

ConfigManager& Core::GetConfig()
{
    return config;
}

ImVec2 Core::Lerp(ImVec2 a, ImVec2 b, float t)
{
    return ImVec2(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    );
}

void Core::Shutdown() {
    config.Save("config.ini");
}

std::time_t Core::ParseDate(const std::string& dateStr)
{
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    return mktime(&tm);
}

std::optional<std::string> Core::GetActiveValue(const std::vector<ValueStartDate>& values, const std::string& currentDateStr)
{
    std::time_t current = ParseDate(currentDateStr);
    std::optional<ValueStartDate> result;

    for (const auto& value : values) {
        std::time_t phaseTime = ParseDate(value.startDate);
        if (phaseTime <= current) {
            if (!result.has_value() || ParseDate(result->startDate) < phaseTime) {
                result = value;
            }
        }
    }

    return result.has_value() ? std::optional<std::string>(result->value) : std::nullopt;
}

bool Core::HasDatePassed(const std::string& inputDate, const std::string& referenceDate)
{
    // Convert "yyyy-mm-dd" to integers for comparison
    std::tm inputTm = {};
    std::tm referenceTm = {};

    std::istringstream issInput(inputDate);
    std::istringstream issRef(referenceDate);

    issInput >> std::get_time(&inputTm, "%Y-%m-%d");
    issRef >> std::get_time(&referenceTm, "%Y-%m-%d");

    if (issInput.fail() || issRef.fail()) {
        // Invalid date format
        return false;
    }

    return std::mktime(&inputTm) > std::mktime(&referenceTm);
}
