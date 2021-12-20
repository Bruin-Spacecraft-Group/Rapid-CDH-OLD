#!/bin/bash

# Setup script

# Find CMake version
CMAKE_VERSION=""
if cmake --version > /dev/null ; then
  CMAKE_VERSION=$(cmake --version | grep -o [0-9].[0-9][-0-9])
  echo "Found CMake $CMAKE_VERSION"
else
  echo "CMake not found; please install CMake"
  exit 1
fi

# Create FindWiringPi.cmake in /usr/share/cmake-<version-number>/Modules
echo -e \
"find_library(WIRINGPI_LIBRARIES NAMES wiringPi)\n"\
"find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)\n\n"\
"include(FindPackageHandleStandardArgs)\n"\
"find_package_handle_standard_args(WiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)"\
> /usr/share/cmake-$CMAKE_VERSION/Modules/FindWiringPi.cmake
echo "Created FindWiringPi.cmake in /usr/share/cmake-$CMAKE_VERSION/Modules"

echo "Setup finished"
