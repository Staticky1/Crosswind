#include "LocalizationManager.h"
#include "LocalizationMacros.h"
#include <sstream>
#include <iostream>
#include <regex>

LocalizationManager& LocalizationManager::Instance() {
    static LocalizationManager instance;
    return instance;
}

using namespace tinyxml2;

bool LocalizationManager::LoadFromFile(const std::string& filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load localization file: " << filename << "\n";
        return false;
    }

    XMLElement* root = doc.FirstChildElement("Localization");
    if (!root) return false;

    for (XMLElement* lang = root->FirstChildElement("Language"); lang; lang = lang->NextSiblingElement("Language")) {
        std::string langCode = lang->Attribute("code");

        // Strings
        for (XMLElement* elem = lang->FirstChildElement("String"); elem; elem = elem->NextSiblingElement("String")) {
            std::string key = elem->Attribute("key");
            std::string value = elem->GetText() ? elem->GetText() : "";
            strings[langCode][key] = value;
        }

        // Plurals
        for (XMLElement* plural = lang->FirstChildElement("Plural"); plural; plural = plural->NextSiblingElement("Plural")) {
            std::string key = plural->Attribute("key");
            PluralForms pf;
            if (XMLElement* one = plural->FirstChildElement("One"))
                pf.one = one->GetText() ? one->GetText() : "";
            if (XMLElement* other = plural->FirstChildElement("Other"))
                pf.other = other->GetText() ? other->GetText() : "";
            plurals[langCode][key] = pf;
        }
    }

    return true;
}

void LocalizationManager::SetLanguage(const std::string& langCode) {
    currentLanguage = langCode;
}

std::string LocalizationManager::GetString(const std::string& key) const {
    auto it = strings.find(currentLanguage);
    if (it != strings.end()) {
        auto strIt = it->second.find(key);
        if (strIt != it->second.end()) {
            return strIt->second;
        }
    }
    return "[[" + key + "]]"; // fallback
}

std::string LocalizationManager::GetPlural(const std::string& key, int count) const {
    auto it = plurals.find(currentLanguage);
    if (it != plurals.end()) {
        auto pluralIt = it->second.find(key);
        if (pluralIt != it->second.end()) {
            const std::string& templateStr = (count == 1) ? pluralIt->second.one : pluralIt->second.other;
            return ReplaceTokens(templateStr, count);
        }
    }
    return "[[" + key + "]]";
}

std::string LocalizationManager::FormatLocalizedString(const std::string key, bool isFemale, const std::unordered_map<std::string, std::string>& extraReplacements)
{
    std::string text = LocalizationManager::Instance().GetString(key);

    // Default pronoun replacements
    std::unordered_map<std::string, std::string> replacements = {
        { "pronoun_he", isFemale ? LocalizationManager::Instance().GetString("pronoun_he_she") : LocalizationManager::Instance().GetString("pronoun_he_he")},
        { "Pronoun_he", isFemale ? LocalizationManager::Instance().GetString("Pronoun_He_She") : LocalizationManager::Instance().GetString("Pronoun_He_He")},
        { "pronoun_him", isFemale ? LocalizationManager::Instance().GetString("pronounhim_her") : LocalizationManager::Instance().GetString("pronounhim_him") },
        { "Pronoun_him", isFemale ? LocalizationManager::Instance().GetString("PronounHim_Her") : LocalizationManager::Instance().GetString("PronounHim_Him") },
        { "his_her",    isFemale ? LocalizationManager::Instance().GetString("pronounhisher_her") : LocalizationManager::Instance().GetString("pronounhisher_his") },
        { "His_her",    isFemale ? LocalizationManager::Instance().GetString("PronounHisHer_Her") : LocalizationManager::Instance().GetString("PronounHisHer_His") },
        { "son_or_daughter", isFemale ? LocalizationManager::Instance().GetString("son_dau_daughter") : LocalizationManager::Instance().GetString("son_dau_son")},
        { "Son_or_daughter", isFemale ? LocalizationManager::Instance().GetString("Son_Dau_Daughter") : LocalizationManager::Instance().GetString("Son_Dau_Son")}
    };

    // Merge in custom replacements
    for (const auto& [k, v] : extraReplacements)
        replacements[k] = v;

    // Replace all {placeholders}
    for (const auto& [placeholder, value] : replacements) {
        std::string pattern = "\\{" + placeholder + "\\}";
        text = std::regex_replace(text, std::regex(pattern), value);
    }

    return text;
}

std::string LocalizationManager::ReplaceTokens(const std::string& text, int count) const {
    std::string result = text;
    size_t pos = result.find("{count}");
    if (pos != std::string::npos) {
        result.replace(pos, 7, std::to_string(count));
    }
    return result;
}
