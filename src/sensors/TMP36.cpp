#include "TMP36.h"

#include "ADS7828.h"

TMP36::TMP36(ADS7828* sensor, int channel) {
	voltageAtZero = 0.5;
	voltagePerDegree = 0.01;
	this->sensor = sensor;
	this->channel = channel;
}

double TMP36::getTemperature(int& err) {
	// using V = vAtZero + T * vPerDeg
	return (sensor->readChannelCommonAnode(channel, err)) * 1000 - voltageAtZero) / voltagePerDegree;
}