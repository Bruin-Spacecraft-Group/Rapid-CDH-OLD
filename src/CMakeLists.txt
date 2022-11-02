cmake_minimum_required(VERSION 3.16)
project(RapidCDH)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "/usr/share/cmake/Modules")

add_compile_options(-Wall)

find_package(WiringPi REQUIRED)

# Include WiringPi headers
include_directories(${WIRINGPI_INCLUDE_DIRS})

add_executable(RapidCDH main.cpp)

add_library(sensors "")
add_library(handlers "")
add_library(util "")

add_subdirectory(sensors)
add_subdirectory(handlers)
add_subdirectory(util)

target_link_libraries(RapidCDH
    PUBLIC
        ${WIRINGPI_LIBRARIES}
        sensors
        handlers
        util
)
