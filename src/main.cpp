#include <chrono>
#include <iostream>
#include <pthread.h>
#include <thread>

#include <wiringPi.h>

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

    std::ofstream fout;
    fout.open("img.jpeg");
    UCamIII ucam(SERIAL_DEV_0, SERIAL_BAUD_RATE, UCAM_RESET_PIN,
                 UCamIII::FMT_JPEG, UCamIII::RAW_160x120, UCamIII::JPEG_640x480, fout);

    try {
        ucam.init();
    } catch (std::runtime_error &e) {
        cout << e.what() << endl;
    }

    fout.close();

    return 0;
}
