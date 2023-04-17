<a id="top"></a>

## Project: Convert to Ceres

The convert2ceres application will convert certain data files to the Ceres data format.

## Overview

* [Cloning project](#Cloning-this-project)
* [Directory Structure](#Directory-Structure)
* [Documentation](#Documentation)
* [Third-party contributions](#third-party-Contributions)
* [Dependencies](#Dependencies)

## Requirements

The Convert2Ceres project is implemented in C++.  It is assumed that the target system has a C++ compiler that supports at least the C++19 standard.  The project also using the CMake meta-build system to generate the build files.  As such, many build systems are supported: makefiles, ninja, Xcode, Visual Studio, etc.

CMake can be installed using your package manager or by visiting https://cmake.org

## Cloning this project

To clone this project to your local machine, use the follow command:

    git clone https://github.com/ripeproject/RAPP_DataPipeline.git

## Directory Structure

### A typical top-level directory layout

    .
    ├── cmake                   # CMake helper files
    ├── src                     # Source files for the C++ code
    └── README.md

## Dependencies

## Third-party Dependencies

The follow support libraries are hosted on GitHub:

* The command line parser library comes from the bjgroup/Lyra project


