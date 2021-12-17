# Cross-compiling for Raspberry Pi with WiringPi
Tested on Pi Zero W v1.1 running Raspbian 11 Bullseye

Most of the work will be done on your host computer. For clarity, whenever something needs to be run on the Pi,
it will be prepended with:

```
pi@raspberrypi ~ $ command
```

## Project structure
- **Root directory**
    - **src** - Source files
      - *CMakeLists.txt* - project build specification
      - *main.cpp*
      - Other source files
    - **build** - Contains all the files that cmake generates
    - *build.sh* - Handy script for automating the entire build process
    - *Toolchain-rpi.cmake* - Raspberry Pi toolchain build specification

## CMake
We're going to be using CMake for this. If you're not familiar with CMake, check out steps 1-3 of
[this tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) to get started.

## Install WiringPi on the Pi

There are a [couple ways](http://wiringpi.com/download-and-install/) to install WiringPi, but to make sure you have the
most recent version, clone it from the [WiringPi Mirror](https://github.com/WiringPi/WiringPi)
(unfortunately Gordon [is no longer maintaining](http://wiringpi.com/wiringpi-deprecated/) the original 
WiringPi library).

```
pi@raspberrypi ~ $ git clone https://github.com/WiringPi/WiringPi.git
```

Then install it with their build script:

```
pi@raspberrypi ~ $ ./build
```

## Get the sample code
Gordon provides a simple [blink example](http://wiringpi.com/examples/blink/) that we will use to test our setup.

Add this to your `src/main.cpp`:

```c++
#include <wiringPi.h>

int main() {
    // GPIO pin for LED
    int LED_PIN = 17;
    
    // Same as wiringPiSetup(), but uses BCM GPIO pins numbers
    wiringPiSetupGpio();
    
    pinMode(LED_PIN, OUTPUT);

    // Blink LED once per second
    while (true) {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }

    return 0;
}
```

## Link WiringPi library
(Taken from [this post](https://stackoverflow.com/questions/30424236/add-wiringpi-lib-to-cmake-on-raspberrypi).)

Add this to `/usr/share/cmake-<version-number>/Modules/FindWiringPi.cmake`:

```cmake
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)
```

Add this to `src/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(project)

find_package(WiringPi REQUIRED)

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS})

add_executable(project main.cpp)

target_link_libraries(project ${WIRINGPI_LIBRARIES})

add_compile_options(-Wall)
```

## Set up cross-compiling
(Based on [this project](https://github.com/Pro/raspi-toolchain).)

### Get the toolchain
This will install the toolchain in `/opt/cross-pi-gcc`.

```
wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
sudo tar xfz raspi-toolchain.tar.gz --strip-components=1 -C /opt
```

This is an old version of GCC, but everything broke when I tried to use a new version.

### Add some more files
Add this to `Toolchain-rpi.cmake`.

```cmake
if("$ENV{RASPBERRY_VERSION}" STREQUAL "")
    set(RASPBERRY_VERSION 1)
else()
    if($ENV{RASPBERRY_VERSION} VERSION_GREATER 3)
        set(RASPBERRY_VERSION 3)
    else()
        set(RASPBERRY_VERSION $ENV{RASPBERRY_VERSION})
    endif()
endif()

# RASPBIAN_ROOTFS should point to the local directory which contains all the libraries and includes from the target raspi.
# Get them with:
# rsync -vR --progress -rl --delete-after --safe-links pi@192.168.1.PI:/{lib,usr,opt/vc/lib} $HOME/rpi/rootfs
# Then RASPBIAN_ROOTFS=$HOME/rpi/rootfs

if("$ENV{RASPBIAN_ROOTFS}" STREQUAL "")
    message(FATAL_ERROR "Define the RASPBIAN_ROOTFS environment variable to point to the raspbian rootfs.")
else()
    set(SYSROOT_PATH "$ENV{RASPBIAN_ROOTFS}")
endif()
set(TOOLCHAIN_HOST "arm-linux-gnueabihf")

message(STATUS "Using sysroot path: ${SYSROOT_PATH}")

set(TOOLCHAIN_CC "${TOOLCHAIN_HOST}-gcc")
set(TOOLCHAIN_CXX "${TOOLCHAIN_HOST}-g++")
set(TOOLCHAIN_LD "${TOOLCHAIN_HOST}-ld")
set(TOOLCHAIN_AR "${TOOLCHAIN_HOST}-ar")
set(TOOLCHAIN_RANLIB "${TOOLCHAIN_HOST}-ranlib")
set(TOOLCHAIN_STRIP "${TOOLCHAIN_HOST}-strip")
set(TOOLCHAIN_NM "${TOOLCHAIN_HOST}-nm")

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSROOT "${SYSROOT_PATH}")

# Define name of the target system
set(CMAKE_SYSTEM_NAME "Linux")
if(RASPBERRY_VERSION VERSION_GREATER 1)
    set(CMAKE_SYSTEM_PROCESSOR "armv7")
else()
    set(CMAKE_SYSTEM_PROCESSOR "arm")
endif()

# Define the compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_CC})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_CXX})

# List of library dirs where LD has to look. Pass them directly through gcc. LD_LIBRARY_PATH is not evaluated by arm-*-ld
set(LIB_DIRS
        "${SYSROOT_PATH}/opt/vc/lib"
        "${SYSROOT_PATH}/lib/${TOOLCHAIN_HOST}"
        "${SYSROOT_PATH}/usr/local/lib"
        "${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}"
        "${SYSROOT_PATH}/usr/lib"
        "${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}/blas"
        "${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}/lapack"
        )
# You can additionally check the linker paths if you add the flags ' -Xlinker --verbose'
set(COMMON_FLAGS "-I${SYSROOT_PATH}/usr/include ")
FOREACH(LIB ${LIB_DIRS})
    set(COMMON_FLAGS "${COMMON_FLAGS} -L${LIB} -Wl,-rpath-link,${LIB}")
ENDFOREACH()

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SYSROOT_PATH}/usr/lib/${TOOLCHAIN_HOST}")

if(RASPBERRY_VERSION VERSION_GREATER 2)
    set(CMAKE_C_FLAGS "-mcpu=cortex-a53 -mfpu=neon-vfpv4 -mfloat-abi=hard ${COMMON_FLAGS}" CACHE STRING "Flags for Raspberry PI 3")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "Flags for Raspberry PI 3")
elseif(RASPBERRY_VERSION VERSION_GREATER 1)
    set(CMAKE_C_FLAGS "-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard ${COMMON_FLAGS}" CACHE STRING "Flags for Raspberry PI 2")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "Flags for Raspberry PI 2")
else()
    set(CMAKE_C_FLAGS "-mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard ${COMMON_FLAGS}" CACHE STRING "Flags for Raspberry PI 1 B+ Zero")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "Flags for Raspberry PI 1 B+ Zero")
endif()

set(CMAKE_FIND_ROOT_PATH "${CMAKE_INSTALL_PREFIX};${CMAKE_PREFIX_PATH};${CMAKE_SYSROOT}")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
```

Add this to `build.sh`:

```shell
#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ! -d $DIR/build ]; then
	mkdir $DIR/build
fi

cd $DIR/build

# To get the rootfs which is required here, use:
# rsync -rl --delete-after --safe-links pi@192.168.1.PI:/{lib,usr} $HOME/rpi/rootfs

export RASPBIAN_ROOTFS=$HOME/rpi/rootfs
export PATH=/opt/cross-pi-gcc/bin:/opt/cross-pi-gcc/libexec/gcc/arm-linux-gnueabihf/8.3.0:$PATH
export RASPBERRY_VERSION=1

cmake -DCMAKE_TOOLCHAIN_FILE=$DIR/Toolchain-rpi.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo $DIR/src
make -j
```

### Copy files from the Pi
We need to link our project with libraries that are also built for the Pi's architecture, so copy these files from 
the Pi and put them in `$HOME/rpi/rootfs`:

- `/usr/lib/`
- `/usr/local/lib/`
- `/usr/local/include/`

I don't think all of these files are necessary, but I'm not sure exactly which ones we need.

## Compile!
To build the executable, just run:

```
./build.sh
```

This will (hopefully) generate an executable and put it in the `build` directory.

## Testing the executable on the Pi

Move our executable to the Pi with `scp` (or `rsync` if you prefer):

```
scp build/project pi@<pi-IP-addr>:/home/pi
```

The default password is `raspberry`.

Then, set up the blink circuit according to [Gordon's example](http://wiringpi.com/examples/blink/) (here is the
[pinout](https://cdn.sparkfun.com/assets/learn_tutorials/6/7/6/PiZero_1.pdf) for the Pi Zero). Execute the program
on the Pi:

```
pi@raspberrypi ~ $ ./project
```

You should see the LED flashing once per second!

## Troubleshooting

1. Sometimes the error that cmake displays may be caused by different error. Always be sure to check `CMakeError.log`
and `CMakeOutput.log` (found in `build/CMakeFiles/`) if you get stuck.