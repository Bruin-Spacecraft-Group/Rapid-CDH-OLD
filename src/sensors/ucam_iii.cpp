#include <iostream>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "ucam_iii.h"

using std::cout;
using std::endl;

UCamIII::UCamIII(const char *serial_dev, const int baud_rate, const int res_pin)
    : res_pin(res_pin) {
    if ((serial_port = serialOpen(serial_dev, baud_rate)) < 0) {
        cout << "Unable to open serial device" << endl;
    }
}

UCamIII::~UCamIII() {
    serialClose(serial_port);
}