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

float Core::DistanceToSegment2D(const Vec3& p, const Vec3& a, const Vec3& b)
{
    Vec3 ap = { p.x - a.x, 0, p.z - a.z };
    Vec3 ab = { b.x - a.x, 0, b.z - a.z };

    float abLenSq = ab.x * ab.x + ab.z * ab.z;
    if (abLenSq == 0.0f) return std::sqrt(ap.x * ap.x + ap.z * ap.z); // a==b

    float t = std::max(0.0f, std::min(1.0f, (ap.x * ab.x + ap.z * ab.z) / abLenSq));

    Vec3 closest = { a.x + ab.x * t, 0, a.z + ab.z * t };

    float dx = p.x - closest.x;
    float dz = p.z - closest.z;
    return std::sqrt(dx * dx + dz * dz);
}

std::pair<int, Vec3> Core::FindClosestPointOnPolyline2D(const Vec3& point, const std::vector<Vec3>& polyline)
{
    float closestDistance = FLT_MAX;
    Vec3 closestPoint = {};
    int closestIndex = -1;

    for (size_t i = 0; i < polyline.size() - 1; ++i)
    {
        float dist = Core::DistanceToSegment2D(point, polyline[i], polyline[i + 1]);
        if (dist < closestDistance)
        {
            closestDistance = dist;
            closestIndex = static_cast<int>(i);
            // Project point onto the segment
            Vec3 a = polyline[i];
            Vec3 b = polyline[i + 1];
            Vec3 ab = { b.x - a.x, 0, b.z - a.z };
            Vec3 ap = { point.x - a.x, 0, point.z - a.z };

            float abLenSq = ab.x * ab.x + ab.z * ab.z;
            float t = (abLenSq > 0.0f) ? std::clamp((ap.x * ab.x + ap.z * ab.z) / abLenSq, 0.0f, 1.0f) : 0.0f;

            closestPoint = { a.x + ab.x * t, 0, a.z + ab.z * t };
        }
    }

    return { closestIndex, closestPoint };
}

float Core::FlipDirection180(float degrees)
{
    // Add 180 and wrap around using modulo
    float flipped = std::fmod(degrees + 180.0f, 360.0f);
    return (flipped < 0.0f) ? flipped + 360.0f : flipped;
}

std::string Core::GetCardinalDirection(float degrees)
{
    static const char* directions[] = {
        "North", "North-East", "East", "South-East",
        "South", "South-West", "West", "North-West"
    };

    // Normalize to 0-360
    degrees = fmod(degrees, 360.0f);
    if (degrees < 0.0f) degrees += 360.0f;

    // Each direction covers 45 degrees
    int index = static_cast<int>((degrees + 22.5f) / 45.0f) % 8;
    return directions[index];
}

float Core::GetPointSideOfSegment(const Vec3& a, const Vec3& b, const Vec3& point)
{
    Vec3 ab = { b.x - a.x, 0, b.z - a.z };
    Vec3 ap = { point.x - a.x, 0, point.z - a.z };
    return Cross2D(ab, ap);
}

std::string Core::CompileNewsStrings(std::vector<std::string>& inStrings)
{
    std::string returnString;
    for (int i = 0; i < inStrings.size(); ++i)
    {        
        if (inStrings[i] != "." && inStrings[i] != "," && i != 0)
        {
            //add space
            returnString += " ";
        }
        returnString += inStrings[i];
    }
    
    return (returnString);
}

std::string Core::FloatToString(float value, int decimals)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals) << value;
    return stream.str();
}

bool Core::StringContains(const std::string& text, const std::string& substring)
{
    return text.find(substring) != std::string::npos;
}

std::filesystem::path Core::GetGamePath()
{
   std::filesystem::path returnpath(config.Get("General.GamePath", "E:/SteamLibrary/steamapps/common/IL-2 Sturmovik Battle of Stalingrad"));
   return returnpath;
}

Vec3 Core::GetRandomPointBetween(const Vec3& a, const Vec3& b)
{
    float t = Core::RandomFloat(0.3f, 0.7f); // Avoid too close to either side (optional)
    return Vec3{
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z)
    };
}

Vec3 Core::GetRandomPointBetweenWithJitter(const Vec3& a, const Vec3& b, float maxOffset)
{
    float t = Core::RandomFloat(0.3f, 0.7f);
    Vec3 base = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z)
    };

    // Apply jitter
    base.x += Core::RandomFloat(-maxOffset, maxOffset);
    base.z += Core::RandomFloat(-maxOffset, maxOffset);

    return base;
}

Vec3 Core::GetRandomPointIn2DRadius(const Vec3& center, float radius)
{
    float angle = Core::RandomFloat(0.0f, 2.0f * 3.14159265f); // Random angle
    float distance = radius * std::sqrt(Core::RandomFloat(0.0f, 1.0f));

    // Convert polar to cartesian (on XZ plane)
    float offsetX = std::cos(angle) * distance;
    float offsetZ = std::sin(angle) * distance;

    return Vec3{ center.x + offsetX, center.y, center.z + offsetZ };
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

std::string Core::GetWeightLabel(int value, int min, int max)
{
    if (max <= min) return "Unknown"; // Safety check

    float range = max - min;
    float normalized = static_cast<float>(value - min) / range;

    if (normalized < 0.33f)
        return "Light";
    else if (normalized < 0.66f)
        return "Medium";
    else
        return "Heavy";
}

std::string Core::ReformatDate(const std::string& isoDate)
{
    if (isoDate.size() != 10 || isoDate[4] != '-' || isoDate[7] != '-')
        return "Invalid Date";

    std::string year = isoDate.substr(0, 4);
    std::string month = isoDate.substr(5, 2);
    std::string day = isoDate.substr(8, 2);

    return day + "/" + month + "/" + year;
}

int Core::GetWeightedRandomIndex(const std::vector<double>& weights)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return dist(gen);
}
int Core::GetWeightedRandomIndex(const std::vector<float>& weights)
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

float Core::RandomFloat(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distrib(min, max);
    return distrib(gen);
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

float Core::Cross2D(const Vec3& a, const Vec3& b)
{
    return a.x * b.z - a.z * b.x;
}

Vec3 Core::Normalize(const Vec3& v)
{
    float len = std::sqrt(v.x * v.x + v.z * v.z);
    return len > 0 ? Vec3{ v.x / len, 0, v.z / len } : Vec3{ 0, 0, 0 };
}

Vec3 Core::Normalize2D(const Vec3& v)
{
    float len = Core::Length2D(v);
    return len > 0 ? Vec3{ v.x / len, 0, v.z / len } : Vec3{ 0, 0, 0 };
}

Vec3 Core::Perpendicular2D(const Vec3& v)
{
    return Vec3{ -v.z, 0, v.x };
}

float Core::Length2D(const Vec3& v)
{
    return std::sqrt(v.x * v.x + v.z * v.z);
}

Vec3 Core::RotateXZ(const Vec3& position, float degrees)
{
    float radians = DegToRad(degrees);

    float cosTheta = std::cos(radians);
    float sinTheta = std::sin(radians);

    float x = position.x * cosTheta - position.z * sinTheta;
    float z = position.x * sinTheta + position.z * cosTheta;

    return Vec3(x, position.y, z);  // Y remains unchanged
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

std::optional<std::string> Core::GetActiveValue(const std::vector<ValueStartDate>& values, const DateTime& currentDateStr)
{
    DateTime current = currentDateStr;
    std::optional<ValueStartDate> result;

    for (const auto& value : values) {
        DateTime phaseTime = value.startDate;
        if (phaseTime < current || phaseTime == current) {
            if (!result.has_value() || result->startDate < phaseTime) {
                result = value;
            }
        }
    }

    return result.has_value() ? std::optional<std::string>(result->value) : std::nullopt;
}

bool Core::HasDatePassed(const DateTime& inputDate, const DateTime& referenceDate)
{
    return inputDate > referenceDate;
}

int Core::GetRandomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}