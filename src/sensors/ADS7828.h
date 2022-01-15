#ifndef ADS7828
#define ADS7828

#include <cstdint>

class ADS7828 {
public:
	ADS7828(const char* device, bool addr0, bool addr1, int& err);
	ADS7828(const char* device, bool addr0, bool addr1, int& err, referenceVoltage);
	double readChannelCommonAnode(int channel, int& err);
	double readChannelDifferentialPair(int pair, bool inverted, int& err);
	double readChannelDifferentialPair(int pair, int& err);
	void setRunning(bool running, int& err);
private:
	uint8_t address;
	double referenceVoltage;
	uint8_t lastCmd;
	int fd;
}

#endif