#include "ina260.h"

using namespace ina260;

//public
Ina260::Ina260(const int devid):c_reg(0x01),v_reg(0x02),p_reg(0x03)
{
	fd = wiringPiI2CSetup(devid);
	if(fd == -1) {std::cout<<"Not open device"<<std::endl; exit(0);} 
}
double
Ina260::readVoltage_mV()
{
	int v_data = wiringPiI2CReadReg16(fd, v_reg);
	v_data = littleEndian(v_data);
	return (double)v_data * 1.25;
}
double 
Ina260::readCurrent_mA()
{
	int c_data = wiringPiI2CReadReg16(fd, c_reg);
	c_data = littleEndian(c_data);
	return (double)c_data * 1.25;	
}
double
Ina260::readPower_mW()
{
	int p_data = wiringPiI2CReadReg16(fd, p_reg);
	p_data = littleEndian(p_data);
	return (double)p_data * 10;
}

// private
short int 
Ina260::littleEndian(short x){
	// little endian each 2 bytes
	short int swap;
	swap=((x & 0xff) << 8 | ((x >> 8) & 0xff));
	return swap;
}
