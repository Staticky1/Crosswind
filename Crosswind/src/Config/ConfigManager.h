#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

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

private:
    std::unordered_map<std::string, std::string> settings;

    static std::string Trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }
};