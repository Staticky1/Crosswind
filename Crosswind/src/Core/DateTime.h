#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <random>
#include "date/date.h"
#include <chrono>

struct DateTime
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;

    static DateTime FromStrings(const std::string& dateStr, const std::string& timeStr = std::string("00:00"))
    {
        DateTime dt;
        sscanf_s(dateStr.c_str(), "%d-%d-%d", &dt.year, &dt.month, &dt.day);
        sscanf_s(timeStr.c_str(), "%d:%d", &dt.hour, &dt.minute);
        return dt;
    }

    std::string ToDateString() const
    {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(4) << year << "-"
            << std::setw(2) << month << "-"
            << std::setw(2) << day;
        return oss.str();
    }

    std::string ToTimeString() const
    {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hour << ":"
            << std::setw(2) << minute;
        return oss.str();
    }

    std::string ToFullString() const
    {
        return ToDateString() + " " + ToTimeString();
    }

    static bool IsOngoing(const DateTime& currentTime, const DateTime& StartingTime, const DateTime& EndingTime)
    {
        return (currentTime >= StartingTime && currentTime < EndingTime);
    }

    void AddMinutes(int minutesToAdd)
    {
       
        date::year_month_day ymd = date::year{ year } / date::month{ unsigned(month) } / date::day{ unsigned(day) };
        if (!ymd.ok()) return;

        std::chrono::sys_days datePart = std::chrono::sys_days{ ymd };
        auto fullTime = datePart + std::chrono::hours{ hour } + std::chrono::minutes{ minute };

        fullTime += std::chrono::minutes{ minutesToAdd };

        auto newDate = std::chrono::floor<std::chrono::days>(fullTime);
        date::year_month_day newYMD = date::year_month_day{ newDate };
        date::hh_mm_ss<std::chrono::minutes> newHMS{ fullTime - newDate };

        year = int(newYMD.year());
        month = unsigned(newYMD.month());
        day = unsigned(newYMD.day());
        hour = int(newHMS.hours().count());
        minute = int(newHMS.minutes().count());
    }

    void AddDays(int daysToAdd)
    {
        AddMinutes(daysToAdd * 24 * 60);
    }

    bool operator<(const DateTime& other) const
    {
        return MinutesSinceEpoch() < other.MinutesSinceEpoch();
    }

    bool operator>(const DateTime& other) const
    {
        return MinutesSinceEpoch() > other.MinutesSinceEpoch();
    }

    bool operator==(const DateTime& other) const
    {
        return year == other.year && month == other.month && day == other.day &&
            hour == other.hour && minute == other.minute;
    }

    bool operator<=(const DateTime& other) const {
        return MinutesSinceEpoch() <= other.MinutesSinceEpoch();
    }

    bool operator>=(const DateTime& other) const {
        return MinutesSinceEpoch() >= other.MinutesSinceEpoch();
    }

    int MinutesSinceEpoch() const
    {
        std::tm t = {};
        t.tm_year = year - 1900;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = minute;
        t.tm_sec = 0;

        std::time_t time = std::mktime(&t);
        return static_cast<int>(time / 60);
    }

    static int RoundToNearest15(int minutes) {
        return static_cast<int>(std::round(minutes / 15.0)) * 15;
    }

    static int TimeStringToMinutes(const std::string& timeStr) {
        int hours, minutes;
        char colon;
        std::istringstream ss(timeStr);
        ss >> hours >> colon >> minutes;
        return hours * 60 + minutes;
    }

    static std::string MinutesToTimeString(int minutes) {
        int hours = minutes / 60;
        int mins = minutes % 60;
        std::ostringstream out;
        out << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << mins;
        return out.str();
    }

    static std::string GetRandomTimeBetween(const std::string& startTime, const std::string& endTime) {
        int startMinutes = TimeStringToMinutes(startTime);
        int endMinutes = TimeStringToMinutes(endTime);

        if (endMinutes < startMinutes)
            std::swap(startMinutes, endMinutes); // handle reverse input

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(startMinutes, endMinutes);

        int randomMinutes = dis(gen);
        int roundedMinutes = RoundToNearest15(randomMinutes);

        // Clamp to ensure rounding doesn't exceed bounds
        roundedMinutes = std::clamp(roundedMinutes, startMinutes, endMinutes);

        return MinutesToTimeString(randomMinutes);
    }

    static void SortTimeStrings(std::vector<std::string>& timeVec) {
        std::sort(timeVec.begin(), timeVec.end(), [](const std::string& a, const std::string& b) {
            return TimeStringToMinutes(a) < TimeStringToMinutes(b);
            });
    }
};