#include <chrono>
#include <iostream>

#include <wiringPi.h>

#include "globals.h"
#include "sensors/UCamIII.h"

using std::cout;
using std::cerr;
using std::endl;

Status ucam_get_raw() {
    std::ofstream fout;
    UCamIII ucam(constants::SERIAL_DEV_0, constants::SERIAL_BAUD_RATE, constants::UCAM_RESET_PIN,
                 UCamIII::FMT_RAW_RGB_16, UCamIII::RAW_160x120, fout);

    Status status = ucam.init();
    if (!status) return status;

    status = ucam.snapshot(UCamIII::SNAP_RAW);
    if (!status) return status;

    uint32_t len;
    status = ucam.get_picture(UCamIII::PIC_SNAPSHOT, len);
    if (!status) return status;

    ucam.write_raw_data(len);

    return SUCCESS;
}

Status ucam_get_jpeg() {
    std::ofstream fout;
    UCamIII ucam(constants::SERIAL_DEV_0, constants::SERIAL_BAUD_RATE, constants::UCAM_RESET_PIN,
                 UCamIII::FMT_JPEG, UCamIII::JPEG_640x480, fout);

    Status status = ucam.init();
    if (!status) return status;

    status = ucam.set_package_size(512);
    if (!status) return status;

    status = ucam.snapshot(UCamIII::SNAP_JPEG);
    if (!status) return status;

    uint32_t len;
    status = ucam.get_picture(UCamIII::PIC_SNAPSHOT, len);
    if (!status) return status;

    return ucam.write_jpeg_data(len);
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
