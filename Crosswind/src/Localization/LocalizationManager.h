#pragma once

#include <string>
#include <unordered_map>
#include <tinyxml2/tinyxml2.h>
#include "Localization/LocalizationMacros.h"

class LocalizationManager {
public:
    // Singleton access
    static LocalizationManager& Instance();

    // Localization logic
    bool LoadFromFile(const std::string& filename);
    void SetLanguage(const std::string& langCode);

    std::string GetString(const std::string& key) const;
    std::string GetPlural(const std::string& key, int count) const;

    std::string FormatLocalizedString(const std::string key, bool isFemale, const std::unordered_map<std::string, std::string>& extraReplacements = {});

private:
    // Hide constructor & disallow copy/move
    LocalizationManager() = default;
    LocalizationManager(const LocalizationManager&) = delete;
    LocalizationManager& operator=(const LocalizationManager&) = delete;

    struct PluralForms {
        std::string one;
        std::string other;
    };

    std::string currentLanguage = "en";
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> strings;
    std::unordered_map<std::string, std::unordered_map<std::string, PluralForms>> plurals;

    std::string ReplaceTokens(const std::string& text, int count) const;
};