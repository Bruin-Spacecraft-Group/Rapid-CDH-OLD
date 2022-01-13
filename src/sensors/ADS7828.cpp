#include "ADS7828.h"

#include <cstdint>
#include <wiringPi.h>
#include <wiringPiI2C.h>

const MAX_READ_VALUE = (1 << 12) - 1;

ADS7828::ADS7828(const char* device, bool addr1, bool addr2, int& err) {
	address = 0b01001000 + 2 * addr1 + addr2;
	fd = wiringPiI2CSetupInterface(device, address);
	if (fd == -1) {
		address = 255;
		err = -1; // I2C setup for this device failed
	}
	else {
		err = 0;
	}
}

uint16_t ADS7828::readChannelCommonAnode(int channel, int& err) {
	if (channel >= 8 || channel < 0) {
		err = -1; // Bad input: invalid channel
		return 0xffff;
	}
	if (fd == -1) {
		err = -1; // I2C setup for this device failed
		return 0xffff;
	}
	uint8_t cmd = 56 * (channel & 1) + 8 * channel + 140;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			err = -1; // I2C write failure to this device
			return 0xffff;
		}
	}
	int res = wiringPiI2CRead(fd);
	if (res < 0 || res > MAX_READ_VALUE) {
		err = -1; // I2C read failure to this device
		return 0xffff;
	}
	err = 0;
	return res;
}

// pair 0 is 0,1; pair 1 is 2,3; pair 2 is 4,5; pair 3 is 6,7
// if the pair is inverted, the positive input is the larger channel number
uint16_t ADS7828::readChannelDifferentialPair(int pair, bool inverted, int& err) {
	if (pair >= 4 || pair < 0) {
		err = -1; // Bad input: invalid pair
		return 0xffff;
	}
	if (fd == -1) {
		err = -1; // I2C setup for this device failed
		return 0xffff;
	}
	uint8_t cmd = 12 + inverted * 64 + pair * 16;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			err = -1; // I2C write failure to this device
			return 0xffff;
		}
	}
	int res = wiringPiI2CRead(fd);
	if (res < 0 || res > MAX_READ_VALUE) {
		err = -1; // I2C read failure to this device
		return 0xffff;
	}
	err = 0;
	return res;
}

uint16_t ADS7828::readChannelDifferentialPair(int pair, int& err) {
	return readChannelDifferentialPair(pair, false, err);
}

void ADS7828::setRunning(bool running, int& err) {
	if (fd == -1) {
		err = -1; // I2C setup for this device failed
		return;
	}
	uint8_t cmd = lastCmd & 0xf0 + running * 0xc;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			err = -1; // I2C write failure to this device
		}
		else {
			lastCmd = cmd;
			err = 0;
		}
	}
}