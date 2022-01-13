#include "ADS7828.h"

#include <cstdint>
#include <wiringPi.h>
#include <wiringPiI2C.h>

// Begin Possible States
const char NOT_INIT[] = "FAILED TO INITIALIZE DEVICE";
const char WORKING[] = "TAKING SAMPLES";
const char ASLEEP[] = "DEVICE TURNED OFF";
const char ERROR[] = "ERROR EXECUTING LAST COMMAND";
// End Possible States

const MAX_READ_VALUE = (1 << 12) - 1;

ADS7828::ADS7828(const char* device, bool addr1, bool addr2) {
	address = 0b01001000 + 2 * addr1 + addr2;
	fd = wiringPiI2CSetupInterface(device, address);
	if (fd == -1) {
		address = 255;
		state = NOT_INIT;
	}
	else {
		state = WORKING;
	}
}

uint16_t ADS7828::readChannelCommonAnode(int channel) {
	if (channel >= 8 || channel < 0 || fd == -1) {
		return 0xffff;
	}
	uint8_t cmd = 56 * (channel & 1) + 8 * channel + 140;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			state = ERROR;
			return 0xffff;
		}
	}
	int res = wiringPiI2CRead(fd);
	if (res < 0 || res > MAX_READ_VALUE) {
		state = ERROR;
		return 0xffff;
	}
	state = WORKING;
	return res;
}

// pair 0 is 0,1; pair 1 is 2,3; pair 2 is 4,5; pair 3 is 6,7
// if the pair is inverted, the positive input is the larger channel number
uint16_t ADS7828::readChannelDifferentialPair(int pair, bool inverted) {
	if (pair >= 4 || pair < 0 || fd == -1) {
		return 0xffff;
	}
	uint8_t cmd = 12 + inverted * 64 + pair * 16;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			state = ERROR;
			return 0xffff;
		}
	}
	int res = wiringPiI2CRead(fd);
	if (res < 0 || res > MAX_READ_VALUE) {
		state = ERROR;
		return 0xffff;
	}
	state = WORKING;
	return res;
}

uint16_t ADS7828::readChannelDifferentialPair(int pair) {
	return readChannelDifferentialPair(pair, false);
}

void ADS7828::setRunning(bool running) {
	if (fd == -1) return;
	uint8_t cmd = lastCmd & 0xf0 + running * 0xc;
	if (cmd != lastCmd) {
		int res = wiringPiI2CWrite(fd, cmd);
		if (res < 0) {
			lastCmd = 255;
			state = ERROR;
		}
		else {
			lastCmd = cmd;
			state = running ? WORKING : ASLEEP;
		}
	}
}

const char* ADS7828::getState() {
	return state;
}