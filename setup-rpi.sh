#!/bin/bash

# Setup script for cross-compiling for Raspberry Pi

# Get rpi rootfs directory
ROOTFS_DIR=""
while getopts 'd:' flag; do
  case "${flag}" in
    d) ROOTFS_DIR="${OPTARG}" ;;
    *) exit 1 ;;
  esac
done

# Find CMake version
CMAKE_VERSION=""
if cmake --version > /dev/null ; then
  CMAKE_VERSION=$(cmake --version | grep -o [0-9].[0-9][-0-9])
  echo "Found CMake $CMAKE_VERSION"
else
  echo "CMake not found; please install CMake"
  exit 1
fi

# Find CMake modules directory
CMAKE_MODULES_DIR=""
if [ -d "/usr/share/cmake/Modules" ]; then
    CMAKE_MODULES_DIR="/usr/share/cmake/Modules"
elif [ -d "/usr/share/cmake-$CMAKE_VERSION/Modules" ]; then
    CMAKE_MODULES_DIR="/usr/share/cmake-$CMAKE_VERSION/Modules"
else
    echo "CMake Modules directory not found"
    exit 1
fi

# Create FindWiringPi.cmake in /usr/share/cmake-<version-number>/Modules
if [ -f "$CMAKE_MODULES_DIR/FindWiringPi.cmake" ]; then
    echo "FindWiringPi.cmake exists; skipping"
else
    echo -e \
    "find_library(WIRINGPI_LIBRARIES NAMES wiringPi)\n"\
    "find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)\n\n"\
    "include(FindPackageHandleStandardArgs)\n"\
    "find_package_handle_standard_args(WiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)"\
    > $CMAKE_MODULES_DIR/FindWiringPi.cmake
    echo "Created FindWiringPi.cmake in $CMAKE_MODULES_DIR"
fi

# Install rpi toolchain
if [ -d "/opt/cross-pi-gcc" ]; then
    echo "/opt/cross-pi-gcc exists; skipping"
else
    # Download rpi toolchain
    if [ -f "./raspi-toolchain.tar.gz" ]; then
        echo "raspi-toolchain.tar.gz exists; skipping"
    else
        echo "Getting the RPi toolchain..."
        wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
        echo "Done"
    fi

    echo "Installing the RPi toolchain to /opt/cross-pi-gcc..."
    tar xzvf raspi-toolchain.tar.gz --strip-components=1 -C /opt
    echo "Done"
fi

# Create directory for rootfs for Raspberry Pi library files
if [ -d "$ROOTFS_DIR/rpi" ]; then
    echo "$ROOTFS_DIR/rpi exists; skipping"
else
    # Download rpi library files
    if [ -f "./rpi-rootfs.tar.gz" ]; then
        echo "rpi-rootfs.tar.gz exists; skipping"
    else
        echo "Getting the RPi library files..."
        wget https://github.com/Bruin-Spacecraft-Group/RapidCDH/releases/latest/download/rpi-rootfs.tar.gz
        echo "Done"
    fi

    mkdir $ROOTFS_DIR/rpi
    echo "Installing the RPi library files in $ROOTFS_DIR/rpi..."
    tar xzvf rpi-rootfs.tar.gz -C $ROOTFS_DIR/rpi
    ln -s $ROOTFS_DIR/rpi/rootfs/usr/lib/ $ROOTFS_DIR/rpi/rootfs/
    echo "Done"
fi

echo "Setup finished"
