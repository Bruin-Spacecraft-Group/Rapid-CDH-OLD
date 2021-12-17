#!/bin/bash

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
"find_package_handle_standard_args(wiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)"\
> /usr/share/cmake-$CMAKE_VERSION/Modules/FindWiringPi.cmake
echo "Created FindWiringPi.cmake in /usr/share/cmake-$CMAKE_VERSION/Modules"

echo "Getting the RPi toolchain..."
wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
echo "Done"

echo "Installing the RPi toolchain to /opt/cross-pi-gcc..."
tar xzvf raspi-toolchain.tar.gz --strip-components=1 -C /opt
echo "Done"

# Create directory for rootfs for Raspberry Pi library files
mkdir $HOME/rpi

echo "Getting the RPi library files..."
wget https://github.com/Bruin-Spacecraft-Group/RapidCDH/releases/latest/download/rpi-rootfs.tar.gz
echo "Done"

echo "Installing the RPi library files in $HOME/rpi..."
tar xzvf rpi-rootfs.tar.gz -C $HOME/rpi
ln -s $HOME/rpi/rootfs/usr/lib/ $HOME/rpi/rootfs/
echo "Done"

echo "Setup finished"