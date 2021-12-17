cmake_minimum_required(VERSION 3.16)
project(RapidCDH)

find_package(WiringPi REQUIRED)

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS})

add_executable(RapidCDH main.cpp)

target_link_libraries(RapidCDH ${WIRINGPI_LIBRARIES})

add_compile_options(-Wall)