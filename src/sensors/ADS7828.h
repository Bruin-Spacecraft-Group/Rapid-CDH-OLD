#ifndef RAPID_CDH_ADS7828_H
#define RAPID_CDH_ADS7828_H

#include <cstdint>

#include "../globals.h"

class ADS7828 {
public:
	ADS7828(const char* device, bool addr0, bool addr1, double referenceVoltage = 2.5);
	[[nodiscard]] Status init();
	[[nodiscard]] Status readChannelCommonAnode(int channel, double& value);
	[[nodiscard]] Status readChannelCommonAnodeRaw(int channel, uint16_t& value);
	[[nodiscard]] Status readChannelDifferentialPair(int pair, bool inverted, double& value);
    [[nodiscard]] Status readChannelDifferentialPair(int pair, double& value);
	[[nodiscard]] Status readChannelDifferentialPairRaw(int pair, bool inverted, uint16_t& value);
	[[nodiscard]] Status readChannelDifferentialPairRaw(int pair, uint16_t& value);
	[[nodiscard]] Status setRunning(bool running);
    double parseRawVoltage(uint16_t raw);
private:
	double referenceVoltage;
	int fd;
	uint8_t lastCmd;
};

#endif