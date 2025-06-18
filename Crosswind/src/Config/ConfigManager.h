#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>

struct CombatSimConfig
{
    int MaxEncountersPerSquadPerSimulatedMission = 2;

    float InjuryChanceOnBail = 0.4f;
    float ModerateInjuryOnBail = 0.3f;
    float MajorInjuryOnBail = 0.1f;

    float InjuryChanceOnCrashLand = 0.5f;
    float ModerateInjuryOnCrashLand = 0.4f;
    float MajorInjuryOnCrashLand = 0.2f;

    float InjuryChanceOnAircraftDamage = 0.4f; // Scales based on aircraft damage level

    int MinorInjuryMaxDays = 5;
    int ModerateInjuryMaxDays = 15;
    int MajorInjuryMaxDays = 30;

    int MinorMaintanceMaxDays = 3;
    int ModerateMaintanceMaxDays = 6;
    int MajorMaintanceMaxDays = 12;
};

class ConfigManager {
public:
    bool Load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line, currentSection;

        while (std::getline(file, line)) {
            // Remove whitespace and ignore comments
            line = Trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;

            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
            }
            else {
                auto eqPos = line.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = Trim(line.substr(0, eqPos));
                    std::string value = Trim(line.substr(eqPos + 1));
                    std::string mapKey = currentSection + "." + key;
                    settings[mapKey] = value;
                }
            }
        }
        return true;
    }

    std::string Get(const std::string& sectionKey, const std::string& defaultValue = "") const {
        auto it = settings.find(sectionKey);
        if (it != settings.end()) return it->second;
        return defaultValue;
    }

    void Set(const std::string& sectionKey, const std::string& value) {
        settings[sectionKey] = value;
    }

    bool Save(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        std::string currentSection;

        for (const auto& kv : settings) {
            auto dotPos = kv.first.find('.');
            if (dotPos == std::string::npos) continue;

            std::string section = kv.first.substr(0, dotPos);
            std::string key = kv.first.substr(dotPos + 1);

            if (section != currentSection) {
                if (!currentSection.empty()) file << "\n";
                file << "[" << section << "]\n";
                currentSection = section;
            }

            file << key << "=" << kv.second << "\n";
        }
        return true;
    }

    void LoadCombatSimConfig(CombatSimConfig& config, const std::string& section = "SimBattleSettings") {
        config.MaxEncountersPerSquadPerSimulatedMission = GetInt(section + ".MaxEncountersPerSquadPerSimulatedMission", combatSimDefaults.MaxEncountersPerSquadPerSimulatedMission);

        config.InjuryChanceOnBail = GetFloat(section + ".InjuryChanceOnBail", combatSimDefaults.InjuryChanceOnBail);
        config.ModerateInjuryOnBail = GetFloat(section + ".ModerateInjuryOnBail", combatSimDefaults.ModerateInjuryOnBail);
        config.MajorInjuryOnBail = GetFloat(section + ".MajorInjuryOnBail", combatSimDefaults.MajorInjuryOnBail);

        config.InjuryChanceOnCrashLand = GetFloat(section + ".InjuryChanceOnCrashLand", combatSimDefaults.InjuryChanceOnCrashLand);
        config.ModerateInjuryOnCrashLand = GetFloat(section + ".ModerateInjuryOnCrashLand", combatSimDefaults.ModerateInjuryOnCrashLand);
        config.MajorInjuryOnCrashLand = GetFloat(section + ".MajorInjuryOnCrashLand", combatSimDefaults.MajorInjuryOnCrashLand);

        config.InjuryChanceOnAircraftDamage = GetFloat(section + ".InjuryChanceOnAircraftDamage", combatSimDefaults.InjuryChanceOnAircraftDamage);

        config.MinorInjuryMaxDays = GetInt(section + ".MinorInjuryMaxDays", combatSimDefaults.MinorInjuryMaxDays);
        config.ModerateInjuryMaxDays = GetInt(section + ".ModerateInjuryMaxDays", combatSimDefaults.ModerateInjuryMaxDays);
        config.MajorInjuryMaxDays = GetInt(section + ".MajorInjuryMaxDays", combatSimDefaults.MajorInjuryMaxDays);

        config.MinorMaintanceMaxDays = GetInt(section + ".MinorMaintanceMaxDays", combatSimDefaults.MinorMaintanceMaxDays);
        config.ModerateMaintanceMaxDays = GetInt(section + ".ModerateMaintanceMaxDays", combatSimDefaults.ModerateMaintanceMaxDays);
        config.MajorMaintanceMaxDays = GetInt(section + ".MajorMaintanceMaxDays", combatSimDefaults.MajorMaintanceMaxDays);
    }

    void SaveCombatSimConfig(const CombatSimConfig& config, const std::string& section = "SimBattleSettings") {
        Set(section + ".MaxEncountersPerSquadPerSimulatedMission", std::to_string(config.MaxEncountersPerSquadPerSimulatedMission));

        Set(section + ".InjuryChanceOnBail", std::to_string(config.InjuryChanceOnBail));
        Set(section + ".ModerateInjuryOnBail", std::to_string(config.ModerateInjuryOnBail));
        Set(section + ".MajorInjuryOnBail", std::to_string(config.MajorInjuryOnBail));

        Set(section + ".InjuryChanceOnCrashLand", std::to_string(config.InjuryChanceOnCrashLand));
        Set(section + ".ModerateInjuryOnCrashLand", std::to_string(config.ModerateInjuryOnCrashLand));
        Set(section + ".MajorInjuryOnCrashLand", std::to_string(config.MajorInjuryOnCrashLand));

        Set(section + ".InjuryChanceOnAircraftDamage", std::to_string(config.InjuryChanceOnAircraftDamage));

        Set(section + ".MinorInjuryMaxDays", std::to_string(config.MinorInjuryMaxDays));
        Set(section + ".ModerateInjuryMaxDays", std::to_string(config.ModerateInjuryMaxDays));
        Set(section + ".MajorInjuryMaxDays", std::to_string(config.MajorInjuryMaxDays));

        Set(section + ".MinorMaintanceMaxDays", std::to_string(config.MinorMaintanceMaxDays));
        Set(section + ".ModerateMaintanceMaxDays", std::to_string(config.ModerateMaintanceMaxDays));
        Set(section + ".MajorMaintanceMaxDays", std::to_string(config.MajorMaintanceMaxDays));
    }

private:
    std::unordered_map<std::string, std::string> settings;

    CombatSimConfig combatSimDefaults;

    static std::string Trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

    int GetInt(const std::string& key, int defaultValue) {
        auto it = settings.find(key);
        if (it != settings.end()) {
            try { return std::stoi(it->second); }
            catch (...) {}
        }
        settings[key] = std::to_string(defaultValue);
        return defaultValue;
    }

    float GetFloat(const std::string& key, float defaultValue) {
        auto it = settings.find(key);
        if (it != settings.end()) {
            try { return std::stof(it->second); }
            catch (...) {}
        }
        settings[key] = std::to_string(defaultValue);
        return defaultValue;
    }


};