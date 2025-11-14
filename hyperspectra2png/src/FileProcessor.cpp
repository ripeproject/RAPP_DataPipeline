
#include "FileProcessor.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id)
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{}

void cFileProcessor::setVnirRgb(float red_nm, float green_nm, float blue_nm)
{
    mVnirRed_nm = red_nm;
    mVnirGreen_nm = green_nm;
    mVnirBlue_nm = blue_nm;
}

void cFileProcessor::setSwirRgb(float red_nm, float green_nm, float blue_nm)
{
    mSwirRed_nm = red_nm;
    mSwirGreen_nm = green_nm;
    mSwirBlue_nm = blue_nm;
}

