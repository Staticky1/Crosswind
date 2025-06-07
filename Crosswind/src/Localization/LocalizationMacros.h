#pragma once


#define LOCAL(key) Core::Instance().GetLocalizationManager().GetString(key).c_str()
#define LOCAL_PLURAL(key, count) Core::Instance().GetLocalizationManager().GetPlural(key, count).c_str()
#define LOCAL_FORMAT(key, bIsFemale, replacements) Core::Instance().GetLocalizationManager().FormatLocalizedString(key,bIsFemale,replacements).c_str();
#define LOCAL_PRONOUN(key, bIsFemale) Core::Instance().GetLocalizationManager().FormatLocalizedString(key,bIsFemale).c_str();