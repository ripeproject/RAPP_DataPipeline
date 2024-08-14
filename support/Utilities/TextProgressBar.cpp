
#include "TextProgressBar.hpp"

#include "StringUtils.hpp"

#define USE_FMT

#ifdef USE_FMT
    #include <fmt/format.h>
    
    using namespace fmt;
#else
    #include <format>
#endif

#include <iostream>
#include <cmath>

using namespace std;


void cTextProgressBar::clear()
{
    mProgressEntries.clear();
}

bool cTextProgressBar::quietMode() const
{
    return mQuietMode;
}

void cTextProgressBar::setQuietMode(bool quiet_mode)
{
    mQuietMode = quiet_mode;
}

bool cTextProgressBar::verboseMode() const
{
    return mVerboseMode;
}

void cTextProgressBar::setVerboseMode(bool verbose_mode)
{
    mVerboseMode = verbose_mode;
}

void cTextProgressBar::setMaxID(int max_id)
{
    mMaxID = max_id;
}

void cTextProgressBar::setFillChars(char half_full, char full)
{
    mFullChar = full;
    mHalfChar = half_full;
}

void cTextProgressBar::setLength(uint8_t length)
{
    mProgressLength = length;
}

void cTextProgressBar::addProgressEntry(int id, const std::string& filename)
{
    if (mQuietMode)
        return;

    std::string str = nStringUtils::compactFilename(filename, 50);
    mProgressEntries[id] = { 0, str };

    std::string line;

    if (mMaxID > 0)
        line = format("({}/{}): ", id+1, mMaxID);

    if (mVerboseMode)
    {
        line += format("{:50s}   0% [", str);

        for (int i = 0; i < mProgressLength; ++i)
            line += mEmptyChar;

        line += "]";
    }
    else
    {
//        line += format("{:50s}   0%", str);
        line += format("{:50s}   ", str);
    }

    mProgressEntries[id].max_length = line.size();

    std::cout << line << std::flush;
}

void cTextProgressBar::addProgressEntry(int id, const std::string& filename, const std::string& prefix)
{
    if (mQuietMode)
        return;

    std::string str = nStringUtils::compactFilename(filename, 50);
    mProgressEntries[id] = { 0, str, prefix };

    std::string line;

    if (mMaxID > 0)
        line = format("({}/{}): ", id + 1, mMaxID);

    if (mVerboseMode)
    {
        line += format("{:50s}  <{:^10s}>   0% [", str, prefix);

        for (int i = 0; i < mProgressLength; ++i)
            line += mEmptyChar;

        line += "]";
    }
    else
    {
//        line += format("{:50s}  <{:^10s}>   0%", str, prefix);
        line += format("{:50s}  <{:^10s}>   ", str, prefix);
    }

    mProgressEntries[id].max_length = line.size();

    std::cout << line << std::flush;
}

void cTextProgressBar::addProgressEntry(int id, const std::string& filename, const std::string& prefix, const std::string& suffix)
{
    if (mQuietMode)
        return;

    std::string str = nStringUtils::compactFilename(filename, 50);
    mProgressEntries[id] = { 0, str, prefix, suffix };

    std::string line;

    if (mMaxID > 0)
        line = format("({}/{}): ", id + 1, mMaxID);

    if (mVerboseMode)
    {
        line += format("{:50s}  <{:^10s}>   0% [", str, prefix);

        for (int i = 0; i < mProgressLength; ++i)
            line += mEmptyChar;

        line += "] " + suffix;
    }
    else
    {
//        line += format("{:50s}  <{:^10s}>   0%", str, prefix);
        line += format("{:50s}  <{:^10s}>   ", str, prefix);
        line += "  " + suffix;
    }

    mProgressEntries[id].max_length = line.size();

    std::cout << line << std::flush;
}


void cTextProgressBar::updateProgressEntry(int id, const std::string& prefix, float progress_pct)
{
    auto& entry = mProgressEntries[id];
    entry.prefix = prefix;

    updateProgressEntry(id, progress_pct);
}

void cTextProgressBar::updateProgressEntry(int id, const std::string& prefix, const std::string& suffix, float progress_pct)
{
    auto& entry = mProgressEntries[id];
    entry.prefix = prefix;
    entry.suffix = suffix;

    updateProgressEntry(id, progress_pct);
}

void cTextProgressBar::updateProgressEntry(int id, float progress_pct)
{
    if (mQuietMode)
        return;

    auto& entry = mProgressEntries[id];

    float integer = 0.0f;
    auto num = (progress_pct / 100.0f) * mProgressLength;
    auto fraction = std::modf(num, &integer);
    int num_fill_char = static_cast<int>(integer);

    bool needs_half_fill = (10.0f * fraction) >= 5.0f;

    if (needs_half_fill)
    {
        needs_half_fill = true;
    }

    if ((num_fill_char == entry.num_fill_char) && (needs_half_fill == entry.needs_half_fill))
        return;

    entry.num_fill_char = num_fill_char;
    entry.needs_half_fill = needs_half_fill;

    std::string line = "\r";

    if (mMaxID > 0)
        line += format("({}/{}): ", id + 1, mMaxID);

    if (mVerboseMode)
    {
        if (entry.prefix.empty())
            line += format("{:50s} {:3}% [", entry.filename, static_cast<int>(progress_pct));
        else
            line += format("{:50s}  <{:^10s}> {:3}% [", entry.filename, entry.prefix, static_cast<int>(progress_pct));

        for (int i = 0; i < num_fill_char; ++i)
            line += mFullChar;

        if (needs_half_fill)
        {
            line += mHalfChar;
            ++num_fill_char;
        }

        for (int i = 0; i < (mProgressLength - num_fill_char); ++i)
            line += mEmptyChar;

        line += "]";
    }
    else
    {
        if (entry.prefix.empty())
        {
//            line += format("{:50s} {:3}% ", entry.filename, static_cast<int>(progress_pct));
            line += format("{:50s}  ", entry.filename);
        }
        else
        {
//            line += format("{:50s}  <{:^10s}> {:3}% ", entry.filename, entry.prefix, static_cast<int>(progress_pct));
            line += format("{:50s}  <{:^10s}>  ", entry.filename, entry.prefix);
        }
    }

    if (!entry.suffix.empty())
    {
        line += " " + entry.suffix;
    }

    if (line.size() > entry.max_length)
    {
        entry.max_length = line.size();
    }
    else if (line.size() < entry.max_length)
    {
        line += std::string(entry.max_length - line.size(), ' ');
    }

    std::cout << line << std::flush;
}

void cTextProgressBar::finishProgressEntry(int id, const std::string& suffix)
{
    auto& entry = mProgressEntries[id];
    entry.suffix = suffix;

    finishProgressEntry(id);
}

void cTextProgressBar::finishProgressEntry(int id, const std::string& prefix, const std::string& suffix)
{
    auto& entry = mProgressEntries[id];
    entry.prefix = prefix;
    entry.suffix = suffix;

    finishProgressEntry(id);
}

void cTextProgressBar::finishProgressEntry(int id)
{
    if (mQuietMode)
        return;

    auto entry = mProgressEntries[id];

    std::string line = "\r";

    if (mMaxID > 0)
        line += format("({}/{}): ", id + 1, mMaxID);

    if (mVerboseMode)
    {
        if (entry.prefix.empty())
            line += format("{:50s}      [", entry.filename);
        else
            line += format("{0:50s}   <{1:^10s}>  [", entry.filename, entry.prefix);

        for (int i = 0; i < mProgressLength; ++i)
            line += mFullChar;

        line += "]";
    }
    else
    {
        if (entry.prefix.empty())
            line += format("{:50s}", entry.filename);
        else
            line += format("{0:50s}   <{1:^10s}>", entry.filename, entry.prefix);
    }

    if (!entry.suffix.empty())
    {
        line += " " + entry.suffix;
    }

    if (line.size() < entry.max_length)
    {
        line += std::string(entry.max_length - line.size(), ' ');
    }

    std::cout << line << std::endl;
}

