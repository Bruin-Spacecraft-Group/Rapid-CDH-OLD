#include "PPG102A6.h"

#include <wiringPi.h>

#include "../globals.h"
#include "ADS7828.h"

PPG102A6::PPG102A6(ADS7828* sensor, int channel, int gpioPin) {
	resistanceAtZero = 1000;
	ppmPerDegree = 3850;
	topVoltage = 5;
	
	// Expected resistance of the RTD at 50 degrees C.
	// Choosing this value maximizes the minimum value for
	// d(Voltage)/d(Temperature) on the range -40 to +50
	// degrees C. If this exact value of resistor cannot be
	// ordered, the real value should be placed here instead.
	dividerResistance = 1192.5;

	this->sensor = sensor;
	this->channel = channel;
	this->gpioPin = gpioPin;

	pinMode(gpioPin, OUTPUT);
	pullUpDnControl(gpioPin, PUD_OFF);
	digitalWrite(gpioPin, LOW);
}

Status PPG102A6::getTemperature(double& value) {
	digitalWrite(gpioPin, HIGH);
	double voltage = topVoltage / 2;
	Status s = sensor->readChannelCommonAnode(channel, voltage);
	digitalWrite(gpioPin, LOW);
	// using V = IR, assuming low-side reference resistor
	double current = voltage / dividerResistance;
	double resistance = (topVoltage - voltage) / current;
	// using R = R0 * (10^6 + ppm * T)
	if (s == SUCCESS) {
		value = (resistance / resistanceAtZero - 1000000) / ppmPerDegree;
	}
	return s;
}