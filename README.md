
# RAPP Data Processing Pipeline

The Ripe Aerial Phenotyping Platform, RAPP, stores data from the various sensors as a stream of raw data.  The data stream needs to undergo a series of processing steps to extract phenotyping data.  This repository contains a series of independently compiled programs to perform a transformation. 

## Overview

* [Cloning project](#Cloning-this-project)
* [Directory Structure](#Directory-Structure)
* [Documentation](#Documentation)
* [Third-party contributions](#third-party-Contributions)
* [Dependencies](#Dependencies)

## Requirements

The OUSTER Connect project is implemented in C++.  It is assumed that the target system has a C++ compiler that supports at least the C++19 standard.  The project also using the CMake meta-build system to generate the build files.  As such, many build systems are supported: makefiles, ninja, Xcode, Visual Studio, etc.

CMake can be installed using your package manager or by visiting https://cmake.org

## Cloning this project

To clone this project to your local machine, use the follow command:

    git clone https://github.com/IGBIllinois/OusterConnect.git


## Directory Structure

### A typical top-level directory layout

    .
    ├── cmake                   # CMake helper files
    ├── docs                    # Documentation and artwork files
    ├── examples                # Example code on how to use the OUSTER Connect libraries
    ├── include                 # The public include files for the libraries
    ├── src                     # Source files for the C++ code
    ├── tests                   # Testing code
    ├── third_party             # Third party C++ libraries
    ├── LICENSE
    └── README.md

## Documentation

* [Definitions](docs/definitions.md#top)
* [Using CMake](docs/cmake.md#top)
* [Building Third Party Libraries](docs/build_third_party_libs.md#top)
* [Build Instructions](docs/build.md#top)

## Third-party Dependencies

The follow support libraries are hosted on GitHub:

* The command line parser library comes from the bjgroup/Lyra project
* The json parser library comes from the nlohmann/json project
* The Catch2 testing library comes from the catchorg/Catch2 project
* The wxWidgets graphical user interface, GUI, library comes from the wxWidgets/wxWidgets project
* The Qt5 graphical user interface, GUI, library comes from the Qt project

The examples require the follow support libraries:

* The ASIO network library found at https://github.com/chriskohlhoff/asio

The testing require the follow support libraries:

* The Eigen linear algebra library found at https://eigen.tuxfamily.org

Please see the "Building third party libraries".
