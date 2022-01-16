#include "ADS7828.h"

#include <cmath>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "../globals.h"

const MAX_READ_VALUE = (1 << 12) - 1;

ADS7828::ADS7828(const char* device, bool addr1, bool addr2, Status& err) {
	address = 0b01001000 + 2 * addr1 + addr2;
	fd = wiringPiI2CSetupInterface(device, address);
	if (fd == -1) {
		address = 255;
		err = I2C_SETUP_FAILURE;
	}
	else {
		err = SUCCESS;
	}
	this->referenceVoltage = 2.5;
}

ADS7828::ADS7828(const char* device, bool addr1, bool addr2, double referenceVoltage, Status& err) {
	address = 0b01001000 + 2 * addr1 + addr2;
	fd = wiringPiI2CSetupInterface(device, address);
	if (fd == -1) {
		address = 255;
		err = I2C_SETUP_FAILURE;
	}
	this->referenceVoltage = referenceVoltage;
}

Status ADS7828::readChannelCommonAnode(int channel, double& value) {
	if (channel >= 8 || channel < 0) {
		return INVALID_INPUT; // Bad input: invalid channel
	}
	if (fd == -1) {
		return I2C_SETUP_FAILURE;
	}
	uint8_t cmd = 56 * (channel & 1) + 8 * channel + 140;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			return I2C_WRITE_FAILURE;
		}
	}
	int res = wiringPiI2CRead(fd);
	if (res < 0 || res > MAX_READ_VALUE) {
		return I2C_READ_FAILURE;
	}
	value = res * referenceVoltage / (1 << 12);
	return SUCCESS;
}

// pair 0 is 0,1; pair 1 is 2,3; pair 2 is 4,5; pair 3 is 6,7
// if the pair is inverted, the positive input is the larger channel number
Status ADS7828::readChannelDifferentialPair(int pair, bool inverted, double& value) {
	if (pair >= 4 || pair < 0) {
		return INVALID_INPUT; // Bad input: invalid pair
	}
	if (fd == -1) {
		return I2C_SETUP_FAILURE;
	}
	uint8_t cmd = 12 + inverted * 64 + pair * 16;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			return I2C_WRITE_FAILURE;
		}
	}
	int res = wiringPiI2CRead(fd);
	if (res < 0 || res > MAX_READ_VALUE) {
		return I2C_READ_FAILURE;
	}
	value = res * referenceVoltage / (1 << 12)
	return SUCCESS;
}

double ADS7828::readChannelDifferentialPair(int pair, double& value) {
	return readChannelDifferentialPair(pair, false, value);
}

Status ADS7828::setRunning(bool running) {
	if (fd == -1) {
		return I2C_SETUP_FAILURE;
	}
	uint8_t cmd = lastCmd & 0xf0 + running * 0xc;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			return I2C_WRITE_FAILURE;
		}
		else {
			lastCmd = cmd;
			return SUCCESS;
		}
	}
}