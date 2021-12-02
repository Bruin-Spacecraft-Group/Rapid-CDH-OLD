#include <iostream>
#include "sensor.h"
#include "../globals.h"
using namespace spartan;

// Constructor
Sensor::Sensor(int busID, int instance)
        : m_busID(busID), m_status(STATUS_OFF), m_instance(instance) {}

// Debug output
void Sensor::printEscapedValues(bool normalize) const {
    int lines = printValues();
    std::cout << "\033[100D" << std::flush;
    std::cout << "\033[" << lines << "A" << std::flush;
    if (normalize) {
        std::cout << "\033[" << lines << "B" << std::flush;
    }
}

// Generic function implementation for the sensor class
int Sensor::getBusID() const { return m_busID; }
int Sensor::getStatus() const { return m_status; }
int Sensor::getInstance() const { return m_instance; }
