#ifndef ADS7828
#define ADS7828

#include <cstdint>

class ADS7828 {
public:
	ADS7828(const char* device, bool addr0, bool addr1, int& err);
	uint16_t readChannelCommonAnode(int channel, int& err);
	uint16_t readChannelDifferentialPair(int pair, bool inverted, int& err);
	uint16_t readChannelDifferentialPair(int pair, int& err);
	void setRunning(bool running, int& err);
private:
	uint8_t address;
	uint8_t lastCmd;
	int fd;
}

#endif