#ifndef INA260
#define INA260

#include <iostream>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <errno.h>
namespace ina260{
class Ina260
{
private:
	const int v_reg;
	const int c_reg;
	const int p_reg;
//	const int config_reg;
	int fd;
	short int littleEndian(short x);
public:
	Ina260(const int devid);
	double readVoltage_mV();
	double readCurrent_mA();
	double readPower_mW();
};
}	// enf namespace ina260
#endif
