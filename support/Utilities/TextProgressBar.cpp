﻿
#include "TextProgressBar.hpp"

#include "StringUtils.hpp"

#include <fmt/core.h>

#include <iostream>
#include <cmath>


using namespace std;
using namespace fmt;


void cTextProgressBar::clear()
{
    mProgressEntries.clear();
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

void cTextProgressBar::addProgressEntry(int id, const std::string& prefix)
{
    std::string str = nStringUtils::compactFilename(prefix, 50);
    mProgressEntries[id] = { 0, str };

    std::string line;

    if (mMaxID > 0)
        line = format("({}/{}): ", id+1, mMaxID);

    line += format("{:50s}   0% [", str);

    for (int i = 0; i < mProgressLength; ++i)
        line += mEmptyChar;

    line += "]";

    std::cout << line << std::flush;
}

void cTextProgressBar::updateProgressEntry(int id, const std::string& prefix, float progress_pct)
{
    auto& entry = mProgressEntries[id];
    entry.prefix = nStringUtils::compactFilename(prefix, 50);

    updateProgressEntry(id, progress_pct);
}

void cTextProgressBar::updateProgressEntry(int id, float progress_pct)
{
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

    line += format("{:50s} {:3}% [", entry.prefix, static_cast<int>(progress_pct));

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

    std::cout << line << std::flush;
}

void cTextProgressBar::finishProgressEntry(int id, const std::string& prefix)
{
    auto& entry = mProgressEntries[id];
    entry.prefix = nStringUtils::compactFilename(prefix, 50);

    finishProgressEntry(id);
}

void cTextProgressBar::finishProgressEntry(int id)
{
    auto entry = mProgressEntries[id];

    std::string line = "\r";

    if (mMaxID > 0)
        line += format("({}/{}): ", id + 1, mMaxID);

    line += format("{:50s}      [", entry.prefix);

    for (int i = 0; i < mProgressLength; ++i)
        line += mFullChar;

    line += "]";

    std::cout << line << std::endl;
}

