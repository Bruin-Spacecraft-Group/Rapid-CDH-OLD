#include <iostream>
#include <string>
#include <cmath>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "UCamIII.h"
#include "../error/UCamIIIException.h"
#include "../error/WiringPiException.h"

using std::cout;
using std::endl;

UCamIII::UCamIII(const char* serial_dev, int baud_rate, int rst_pin,
                 uint8_t img_format, uint8_t raw_res, uint8_t jpeg_res, std::ofstream& fout)
    : m_serial_dev(serial_dev), m_baud_rate(baud_rate), m_serial_port(0), m_rst_pin(rst_pin),
        m_pkg_size(0), m_img_format(img_format), m_raw_res(raw_res), m_jpeg_res(jpeg_res), m_fout(fout) {}

UCamIII::~UCamIII() {
    serialClose(m_serial_port);
}

void UCamIII::init() {
    // Open serial connection
    if ((m_serial_port = serialOpen(m_serial_dev, m_baud_rate)) < 0) {
        throw WiringPiException(("Unable to open serial device: " + string(m_serial_dev)));
    }

    // Synchronization
    hardware_reset();
    sync();

    // Allow AGC and AEC circuits to stabilize
    delay(2000);

    // Configuration
    send_cmd(CMD_INITIAL, 0, m_img_format, m_raw_res, m_jpeg_res);
}

void UCamIII::hardware_reset() const {
    pinMode(m_rst_pin, OUTPUT);
    digitalWrite(m_rst_pin, LOW);
    delay(10);
    pinMode(m_rst_pin, INPUT);
    delay(10);
}

void UCamIII::send_cmd_unchecked(int cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    uint8_t data[NUM_CMD_BYTES] = {CMD_PREFIX, (uint8_t) cmd, param1, param2, param3, param4};
    for (uint8_t i : data) {
        serialPutchar(m_serial_port, i);
    }

    cout << "UCAM: Sent: ";
    for (unsigned char i : data) {
        cout << i;
    }
    cout << endl;
}

void UCamIII::send_cmd(int cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    send_cmd_unchecked(cmd, param1, param2, param3, param4);

    // Check response
    int data[NUM_CMD_BYTES];
    receive_cmd(data);
    if (data[0] == CMD_PREFIX && data[1] == CMD_NAK) {
        throw UCamIIIException(data[4]);
    } else if (data[0] != CMD_PREFIX && data[1] != CMD_ACK && data[2] != cmd) {
        throw UCamIIIException("Command verification not received " + std::to_string(cmd));
    }
}

void UCamIII::receive_cmd(int* data) const {
    int i = 0;
    while (serialDataAvail(m_serial_port) > 0 && i < NUM_CMD_BYTES) {
        data[i] = serialGetchar(m_serial_port);
        i++;
    }

    if (i == NUM_CMD_BYTES) {
        cout << "UCAM: Received: ";
        for (int j = 0; j < NUM_CMD_BYTES; j++) {
            cout << data[j];
        }
        cout << endl;
    } else {
        cout << "UCAM: No data received" << endl;
    }
}

void UCamIII::sync() const {
    int data[NUM_CMD_BYTES];

    // Send SYNC command until ACK command is received
    for (int i = 0; i < MAX_TRIES; i++) {
        // Send SYNC command
        send_cmd(CMD_SYNC);

        // Check if ACK command received
        receive_cmd(data);
        if (data[0] == CMD_PREFIX && data[1] == CMD_ACK && data[2] == CMD_SYNC) {
            // Check if SYNC command received
            receive_cmd(data);
            if (data[0] == CMD_PREFIX && data[1] == CMD_SYNC) {
                // Respond with ACK command
                send_cmd_unchecked(CMD_ACK, CMD_SYNC);
                return;
            }
        }

        delay(5 + i);
    }

    throw UCamIIIException("Synchronization failed after " + std::to_string(MAX_TRIES) + " tries");
}

void UCamIII::initial(uint8_t img_format, uint8_t raw_res, uint8_t jpeg_res) {
    send_cmd(CMD_INITIAL, 0, img_format, raw_res, jpeg_res);

    m_img_format = img_format;
    m_raw_res = raw_res;
    m_jpeg_res = jpeg_res;
}

void UCamIII::set_package_size(int size) {
    send_cmd(CMD_SET_PACKAGE_SIZE, 0x8, (size & 0xFF), (size >> 8) & 0xFF);

    if (size > MAX_PKG_SIZE) {
        m_pkg_size = MAX_PKG_SIZE;
    } else {
        m_pkg_size = size;
    }
}

void UCamIII::snapshot(uint8_t snapshot_type, int skipped_frames) const {
    send_cmd(CMD_SNAPSHOT, SNAP_JPEG, (skipped_frames & 0xFF), (skipped_frames >> 8) & 0xFF);
}

int UCamIII::get_picture(uint8_t picture_type) const {
    int data[NUM_CMD_BYTES];

    send_cmd(CMD_GET_PICTURE, picture_type);

    receive_cmd(data);
    if (data[0] != CMD_PREFIX || data[1] != CMD_DATA || data[2] != picture_type) {
        throw UCamIIIException("Improper DATA response from GET PICTURE");
    }

    return data[3] + (data[4] << 8) + (data[5] << 16);
}

void UCamIII::write_jpeg_data(int len) const {
    int num_pkgs = std::ceil(len / (m_pkg_size - 6));
    int pkg_id, data_len, sum, verify_code;
    uint8_t byte;
    char img_data[m_pkg_size - 6];

    send_cmd_unchecked(CMD_ACK);

    // Receive image data packages
    for (int i = 1; i <= num_pkgs; i++) {
        // Reset variables
        pkg_id = 0;
        data_len = 0;
        verify_code = 0;
        sum = 0;

        // Get first 2 bytes: package ID
        byte = serialGetchar(m_serial_port);
        sum += byte;
        pkg_id += byte;

        byte = serialGetchar(m_serial_port);
        sum += byte;
        pkg_id += byte << 8;

        if (pkg_id != i) {
            throw UCamIIIException("Mismatched JPEG data packages");
        }

        // Get next 2 bytes: data length
        byte = serialGetchar(m_serial_port);
        sum += byte;
        data_len += byte;

        byte = serialGetchar(m_serial_port);
        sum += byte;
        data_len += byte << 8;

        // Collect image data
        for (int j = 0; j < data_len; j++) {
            byte = serialGetchar(m_serial_port);
            sum += byte;
            img_data[j] = (char) byte;
        }

        // Get last 2 byte: verify code
        verify_code += serialGetchar(m_serial_port);
        verify_code += serialGetchar(m_serial_port) << 8;
        if (verify_code != (sum & 0xFF)) {
            throw UCamIIIException("JPEG data package verification failed");
        }

        // Write image data
        m_fout.write(img_data, data_len);

        // Package received successfully
        if (i == num_pkgs) {
            // Final package success
            send_cmd_unchecked(CMD_ACK, 0, 0, 0xF0, 0xF0);
        } else {
            send_cmd_unchecked(CMD_ACK, 0, 0, (i & 0xFF), (i >> 8));
        }
    }

    serialFlush(m_serial_port);
}

void UCamIII::write_raw_data(int len) const {
    char byte;

    send_cmd_unchecked(CMD_ACK);

    for (int i = 0; i < len; i++) {
        if (serialDataAvail(m_serial_port) > 0) {
            byte = (char) serialGetchar(m_serial_port);
            m_fout.write(&byte, 1);
        } else {
            delay(5);
        }
    }

    // Data received successfully
    send_cmd_unchecked(CMD_ACK, CMD_DATA, 0, 1);

    serialFlush(m_serial_port);
}