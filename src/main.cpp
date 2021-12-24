#include <chrono>
#include <iostream>
#include <pthread.h>
#include <thread>

#include <wiringPi.h>

#include "globals.h"
#include "sensors/UCamIII.h"

using std::cout;
using std::cerr;
using std::endl;

void ucam_get_raw() {
    std::ofstream fout;
    UCamIII ucam(SERIAL_DEV_0, SERIAL_BAUD_RATE, UCAM_RESET_PIN,
                 UCamIII::FMT_RAW_RGB_16, UCamIII::RAW_160x120, fout);

    ucam.init();
    ucam.snapshot(UCamIII::SNAP_RAW);
    uint32_t len = ucam.get_picture(UCamIII::PIC_SNAPSHOT);
    ucam.write_raw_data(len);
}

void ucam_get_jpeg() {
    std::ofstream fout;
    UCamIII ucam(SERIAL_DEV_0, SERIAL_BAUD_RATE, UCAM_RESET_PIN,
                 UCamIII::FMT_JPEG, UCamIII::JPEG_640x480, fout);

    ucam.init();
    ucam.set_package_size(512);
    ucam.snapshot(UCamIII::SNAP_JPEG);
    uint32_t len = ucam.get_picture(UCamIII::PIC_SNAPSHOT);
    ucam.write_jpeg_data(len);
}

int main() {
    if (wiringPiSetup() == -1) {
        cerr << "Unable to start WiringPi" << endl;
        return 1;
    } else {
        cout << "WiringPi set up" << endl;
    }

    return 0;
}
