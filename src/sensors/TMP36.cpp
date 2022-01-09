#include "TMP36.h"

#include "ina260.h"

TMP36::TMP36(ina260* sensor) : voltageAtZero(0.5), voltagePerDegree(0.01), sensor(sensor) {}

double TMP36::getTemperature() {
	// using V = vAtZero + T * vPerDeg
	return (sensor->readVoltage_mV() * 1000 - voltageAtZero) / voltagePerDegree;
}