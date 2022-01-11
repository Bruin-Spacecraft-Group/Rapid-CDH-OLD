#ifndef ADS7828
#define ADS7828

#include <cstdint>

class ADS7828 {
public:
	ADS7828(bool addr0, bool addr1);
	uint16_t readChannelCommonAnode(int channel);
	uint16_t readChannelDifferentialPair(int pair, bool inverted);
	uint16_t readChannelDifferentialPair(int pair);
	void setRunning(bool running);
	const char* getState();
private:
	uint8_t address;
	const char* state;
	uint8_t lastCmd;
	int fd;
}

#endif