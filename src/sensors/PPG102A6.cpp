#include "PPG102A6.h"

#include "ina260.h"

PPG102A6::PPG102A6(ina260* sensor) : resistanceAtZero(1000), ppmPerDegree(3850), sensor(sensor) {}

double PPG102A6::getTemperature() {
	// using R = R0 * (10^6 + ppm * T) and V = IR
	double resistance = sensor->readVoltage_mV() / sensor->readCurrent_mA();
	return (resistance / resistanceAtZero - 1000000) / ppmPerDegree;
}