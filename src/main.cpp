#include <chrono>
#include <iostream>
#include <pthread.h>
#include <thread>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "globals.h"
#include "sensors/UCamIII.h"

using std::cout;
using std::endl;

int main() {
    if (wiringPiSetup() == -1) {
        cout << "Unable to start WiringPi" << endl;
        return 1;
    } else {
        cout << "WiringPi set up" << endl;
    }

    UCamIII ucam(SERIAL_DEV_0, SERIAL_BAUD_RATE, UCAM_RESET_PIN);

    try {
        ucam.initialize();
    } catch (UCamIIIException &e) {
        cout << "UCam initialization failed" << endl;
    }

    return 0;
}
