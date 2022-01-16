#include "TMP36.h"

#include "ADS7828.h"

TMP36::TMP36(ADS7828* sensor, int channel) {
	voltageAtZero = 0.5;
	voltagePerDegree = 0.01;
	this->sensor = sensor;
	this->channel = channel;
}

Status TMP36::getTemperature(double& value) {
	// using V = vAtZero + T * vPerDeg
	double voltage;
	Status s = sensor->readChannelCommonAnode(channel, value));
	if (s == SUCCESS) {
		value = (voltage * 1000 - voltageAtZero) / voltagePerDegree;
	}
	return s;
}