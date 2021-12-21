#include <iostream>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "ucam_iii.h"

using std::cout;
using std::endl;

UCamIII::UCamIII(const char* serial_dev, const int baud_rate, const int rst_pin)
    : serial_dev(serial_dev), baud_rate(baud_rate), rst_pin(rst_pin) {}

UCamIII::~UCamIII() {
    serialClose(serial_port);
}

int UCamIII::initialize() {
    // Initialize reset pin
    pinMode(rst_pin, OUTPUT);

    // Open serial connection
    if ((serial_port = serialOpen(serial_dev, baud_rate)) < 0) {
        cout << "UCAM: Unable to open serial device" << endl;
        return 1;
    } else {
        cout << "UCAM: opened serial device" << endl;
    }

    return sync();
}

void UCamIII::send_cmd(CmdID cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    uint8_t data[6] = {0xAA, cmd, param1, param2, param3, param4};
    for (int i = 0; i < 6; i++) {
        serialPutchar(serial_port, (unsigned char) data[i]);
    }

    cout << "UCAM: Sent: ";
    for (int i = 0; i < 6; i++) {
        cout << data[i];
    }
    cout << endl;
}

void UCamIII::receive_cmd(int* data) const {
    int i = 0;
    while (serialDataAvail(serial_port) > 0) {
        data[i] = serialGetchar(serial_port);
        i++;
    }

    if (i > 0) {
        cout << "UCAM: Received: ";
        for (int j = 0; j < i; j++) {
            cout << data[j];
        }
        cout << endl;
    } else {
        cout << "UCAM: No data received" << endl;
    }
}

void UCamIII::hardware_reset() const {
    digitalWrite(rst_pin, LOW);
    delay(5);
    digitalWrite(rst_pin, HIGH);
}

int UCamIII::sync() const {
    int data[6];

    hardware_reset();

    // Send SYNC command until ACK command is received
    for (int i = 0; i < 60; i++) {
        // Send SYNC command
        UCamIII::send_cmd(UCamIII::CMD_SYNC, 0, 0, 0, 0);

        delay(5 + i);

        // Check if ACK command received
        UCamIII::receive_cmd(data);
        if (data[0] == 0xAA && data[1] == UCamIII::CMD_ACK && data[2] == UCamIII::CMD_SYNC) {
            // Respond with ACK command
            UCamIII::send_cmd(UCamIII::CMD_ACK, UCamIII::CMD_SYNC, 0, 0, 0);

            // Allow AGC and AEC circuits to stabilize
            delay(1000);

            break;
        }
    }

    return 1;
}