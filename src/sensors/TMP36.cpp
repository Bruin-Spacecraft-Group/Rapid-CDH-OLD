#include "TMP36.h"

#include "ADS7828.h"

TMP36::TMP36(ADS7828* sensor) : voltageAtZero(0.5), voltagePerDegree(0.01), sensor(sensor) {}

double TMP36::getTemperature() {
	// using V = vAtZero + T * vPerDeg
	return (sensor->readChannelCommonAnode(channel)) * 1000 - voltageAtZero) / voltagePerDegree;
}