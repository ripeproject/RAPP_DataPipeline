
#include "BIL_2_PNG.hpp"

#include "Constants.hpp"
#include "MathUtils.hpp"
#include "StringUtils.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <cctype>


// Reads a BIL header file into a map
bool readBILHeader(const std::string& filename, std::map<std::string, std::string>& headerMap) 
{
    using namespace nStringUtils;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open header file: " << filename << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        line = strim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') 
        {
            continue; // skip comments and empty lines
        }

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) 
        {
            continue; // skip malformed lines
        }

        std::string key = toLower(strim(line.substr(0, eqPos)));
        std::string value = strim(line.substr(eqPos + 1));

        if (key == "wavelength")
        {
            auto first = value.find_first_of('{');
            auto last = value.find_last_of('}');

            if ((first != std::string::npos) && (last != std::string::npos))
            {
                value = strim(value.substr(first + 1, last - first));
            }
        }

        headerMap[key] = value;
    }

    return true;
}


// Reads a BIL file into a 3D vector: data[band][row][col]
template <typename T>
std::vector<std::vector<std::vector<T>>> readBIL(const std::string& filename, size_t rows, size_t cols, size_t bands)
{
    // Open file in binary mode
    std::ifstream file(filename, std::ios::binary);
    if (!file) 
    {
        throw std::runtime_error("Error: Cannot open file " + filename);
    }

    // Allocate memory: bands × rows × cols
    std::vector<std::vector<std::vector<T>>> data(
        bands, std::vector<std::vector<T>>(rows, std::vector<T>(cols)));

    // Read line by line
    for (size_t row = 0; row < rows; ++row) 
    {
        for (size_t band = 0; band < bands; ++band) 
        {
            file.read(reinterpret_cast<char*>(data[band][row].data()), cols * sizeof(T));

            if (!file) 
            {
                throw std::runtime_error("Error: Unexpected end of file while reading.");
            }
        }
    }

    return data;
}


std::vector<float> toArray(const std::string& in)
{
    std::vector<float> out;

    if (in.empty())
        return out;

    std::string::size_type first = 0;
    std::string::size_type last = in.find_first_of(',');

    while (last < std::string::npos)
    {
        out.push_back(std::stof(in.substr(first, last - first)));

        first = last + 1;
        last = in.find(',', first);
    }

    out.push_back(std::stof(in.substr(first, last - first)));

    return out;
}

cBIL_2_Png::cBIL_2_Png()
{
}

cBIL_2_Png::~cBIL_2_Png()
{
    mImage.release();
}

bool cBIL_2_Png::loadHeader(const std::string& in)
{
    return readBILHeader(in, mHeader);
}

std::pair<float, float> cBIL_2_Png::getWavelengthRange() const
{
    std::pair<float, float> range = { 0.0f, 0.0f };

    if (mHeader.contains("wavelength"))
    {
        std::string wavelengths = mHeader.at("wavelength");

        auto first = wavelengths.find_first_of(',');
        if (first != std::string::npos)
        {
            range.first = std::stof(wavelengths.substr(0, first));
        }

        auto last = wavelengths.find_last_of(',');
        if (last != std::string::npos)
        {
            range.second = std::stof(wavelengths.substr(last+1));
        }
    }

    return range;
}

void cBIL_2_Png::setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm)
{
    mRed_nm = red_nm;
    mGreen_nm = green_nm;
    mBlue_nm = blue_nm;

    mRedIndex = 279;
    mGreenIndex = 169;
    mBlueIndex = 69;

    float red_diff = 10000.0f;
    float green_diff = 10000.0f;
    float blue_diff = 10000.0f;

    auto wavelengths_nm = toArray(mHeader.at("wavelength"));

    for (std::size_t i = 0; i < wavelengths_nm.size(); ++i)
    {
        float wavelength = wavelengths_nm[i];

        if (std::abs(wavelength - mRed_nm) < red_diff)
        {
            red_diff = std::abs(wavelength - mRed_nm);
            mRedIndex = i;
        }

        if (std::abs(wavelength - mGreen_nm) < green_diff)
        {
            green_diff = std::abs(wavelength - mGreen_nm);
            mGreenIndex = i;
        }

        if (std::abs(wavelength - mBlue_nm) < blue_diff)
        {
            blue_diff = std::abs(wavelength - mBlue_nm);
            mBlueIndex = i;
        }
    }
}

void cBIL_2_Png::writeRgbImage(std::filesystem::path in, std::filesystem::path out)
{
    auto bands = std::stoi(mHeader.at("bands"));
    auto lines = std::stoi(mHeader.at("lines"));
    auto samples = std::stoi(mHeader.at("samples"));
    auto data_type = std::stoi(mHeader.at("data type"));

    mImage.create(lines, samples, CV_16UC4);

    int maxPixelValue = std::pow(2, data_type);

    mColorScale = 255.0 / maxPixelValue;
    mColorScale = 65535.0 / maxPixelValue;
    mColorScale *= 4.0;

    auto bil = readBIL<unsigned short>(in.string(), lines, samples, bands);

    const auto& red_slice = bil[mRedIndex];
    const auto& green_slice = bil[mGreenIndex];
    const auto& blue_slice = bil[mBlueIndex];

    for (int l = 0; l < lines; ++l)
    {
        const auto& red = red_slice[l];
        const auto& green = green_slice[l];
        const auto& blue = blue_slice[l];

        for (int i = 0; i < samples; ++i)
        {
            uint16_t r = nMathUtils::bound<uint16_t>(red[i] * mColorScale);
            uint16_t g = nMathUtils::bound<uint16_t>(green[i] * mColorScale);
            uint16_t b = nMathUtils::bound<uint16_t>(blue[i] * mColorScale);

            uint16_t a = 65535;

            if (r == 0 && b == 0 && g == 0)
                a = 0;

            cv::Point p(i, l);
            mImage.at<cv::Vec4w>(p) = { b,g,r,a };
        }
    }

    cv::String name = out.string();
    cv::imwrite(name, mImage);
}


