cmake_minimum_required(VERSION 3.16)
project(RapidCDH)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall)

find_package(WiringPi REQUIRED)

# Include WiringPi headers
include_directories(${WIRINGPI_INCLUDE_DIRS})

add_executable(RapidCDH main.cpp)

add_library(sensorsLib "")

add_subdirectory(sensors)

target_link_libraries(RapidCDH PUBLIC ${WIRINGPI_LIBRARIES} sensorsLib)