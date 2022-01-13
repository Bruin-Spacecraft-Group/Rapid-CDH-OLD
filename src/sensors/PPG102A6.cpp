#include "PPG102A6.h"

#include <wiringPi.h>

PPG102A6::PPG102A6(ADS7828* sensor, int channel, int gpioPin) {
	resistanceAtZero = 1000;
	ppmPerDegree = 3850;
	this->sensor = sensor;
	// Expected resistance of the RTD at 50 degrees C.
	// Choosing this value maximizes the minimum value for
	// d(Voltage)/d(Temperature) on the range -40 to +50
	// degrees C. If this exact value of resistor cannot be
	// ordered, the real value should be used.
	this->dividerResistance = 1192.5;

	this->channel = channel;
	this->gpioPin = gpioPin;

	pinMode(gpioPin, OUTPUT);
	pullUpDnControl(gpioPin, PUD_OFF);
	digitalWrite(gpioPin, LOW);
}

double PPG102A6::getTemperature() {
	digitalWrite(gpioPin, HIGH);
	double voltage = sensor->readChannelCommonAnode(channel);
	digitalWrite(gpioPin, LOW);
	// using V = IR
	double current = voltage / dividerResistance;
	double resistance = voltage / current;
	// using R = R0 * (10^6 + ppm * T)
	return (resistance / resistanceAtZero - 1000000) / ppmPerDegree;
}