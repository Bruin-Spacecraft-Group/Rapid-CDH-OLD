#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "UCamIII.h"

using std::cout;
using std::cerr;
using std::endl;

UCamIII::UCamIII(const char* serial_dev, uint32_t baud_rate, uint8_t rst_pin,
                 uint8_t img_format, uint8_t resolution, std::ofstream& fout)
    : m_serial_dev(serial_dev), m_baud_rate(baud_rate), m_serial_port(0), m_rst_pin(rst_pin),
        m_pkg_size(0), m_img_format(img_format), m_resolution(resolution), m_fout(fout) {}

UCamIII::~UCamIII() {
    serialClose((int) m_serial_port);
}

void UCamIII::init() {
    pinMode(m_rst_pin, OUTPUT);
    digitalWrite(m_rst_pin, HIGH);

    // Open serial connection
    if ((m_serial_port = serialOpen(m_serial_dev, (int) m_baud_rate)) < 0) {
        cerr << "Unable to open serial device: " << std::string(m_serial_dev) << endl;
        return;
    }

    // Synchronization
    hard_reset();
    sync();

    // Allow AGC and AEC circuits to stabilize
    delay(2000);

    // Configuration
    send_cmd(CMD_INITIAL, 0x00, m_img_format, m_resolution, m_resolution);
}

void UCamIII::sync() const {
    uint8_t data[NUM_CMD_BYTES];

    // Send SYNC command until ACK command is received
    for (uint8_t i = 0; i < MAX_TRIES; i++) {
        // Send SYNC command
        send_cmd_unchecked(CMD_SYNC);

        // Check if ACK command received
        receive_cmd(data, NUM_CMD_BYTES, 5 + i);
        if (data[0] == CMD_PREFIX && data[1] == CMD_ACK && data[2] == CMD_SYNC) {
            // Check if SYNC command received
            receive_cmd(data, NUM_CMD_BYTES, 5 + i);
            if (data[0] == CMD_PREFIX && data[1] == CMD_SYNC) {
                delay(10);
                // Respond with ACK command
                send_cmd_unchecked(CMD_ACK, CMD_SYNC);

                cout << "Synchronization completed" << endl;
                return;
            }
        }
    }

    cerr << "Synchronization failed after " << MAX_TRIES << " tries" << endl;
}

void UCamIII::hard_reset() const {
    digitalWrite(m_rst_pin, LOW);
    delay(10);
    digitalWrite(m_rst_pin, HIGH);
    delay(10);
}

void UCamIII::soft_reset(uint8_t rst_type, bool immediate) const {
    if (immediate) {
        send_cmd(CMD_RESET, rst_type, 0x00, 0x00, 0xFF);
    } else {
        send_cmd(CMD_RESET, rst_type);
    }
}

void UCamIII::send_cmd_unchecked(uint8_t cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    uint8_t data[NUM_CMD_BYTES] = {CMD_PREFIX, cmd, param1, param2, param3, param4};
    for (uint8_t i : data) {
        serialPutchar((int) m_serial_port, i);
    }

    cout << "UCam Sent: ";
    print_cmd(data);
}

void UCamIII::send_cmd(uint8_t cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    send_cmd_unchecked(cmd, param1, param2, param3, param4);

    // Check response
    uint8_t data[NUM_CMD_BYTES];
    receive_cmd(data);
    if (data[0] == CMD_PREFIX && data[1] == CMD_NAK) {
        cerr << parse_nak_err(data[4]) << endl;
        return;
    } else if (data[0] != CMD_PREFIX || data[1] != CMD_ACK || data[2] != cmd) {
        cerr << "Command verification not received for " << cmd_to_str(cmd) << endl;
        return;
    }
}

void UCamIII::receive_cmd(uint8_t * data, uint8_t len, uint16_t timeout) const {
    uint8_t i = 0;
    uint current_time = millis();
    while (i < len && (millis() - current_time) < (uint) timeout) {
        while (serialDataAvail((int) m_serial_port) > 0 && i < len) {
            data[i] = (uint8_t) serialGetchar((int) m_serial_port);
            i++;
        }
    }

    if (i < len) {
        cerr << "UCam: Serial receive timeout: " << timeout << " ms" << endl;
    } else {
        cout << "UCam Received " << (int) i << " bytes: ";
        print_cmd(data, i);
    }
}

void UCamIII::initial(uint8_t img_format, uint8_t resolution) {
    send_cmd(CMD_INITIAL, 0x00, img_format, resolution, resolution);

    m_img_format = img_format;
    m_resolution = resolution;
}

void UCamIII::set_package_size(uint32_t size) {
    send_cmd(CMD_SET_PACKAGE_SIZE, 0x08, (size & 0xFF), (size >> 8) & 0xFF);

    if (size > MAX_PKG_SIZE) {
        m_pkg_size = MAX_PKG_SIZE;
    } else {
        m_pkg_size = size;
    }
}

void UCamIII::set_baud_rate(uint32_t baud_rate) {
    uint8_t first_divider, second_divider = 0;

    switch (baud_rate) {
        case 2400:
        case 4800:
        case 9600:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
            first_divider = 31;
            break;
        case 153600:
        case 230400:
        case 460800:
            first_divider = 7;
            break;
        case 921600:
        case 1843200:
            first_divider = 1;
            break;
        case 1228800:
            first_divider = 2;
            break;
        case 3686400:
            first_divider = 0;
            break;
        default:
            cerr << "Unsupported baud rate: " << baud_rate << endl;
            return;
    }

    switch (baud_rate) {
        case 2400:
            second_divider = 47;
            break;
        case 4800:
            second_divider = 23;
            break;
        case 9600:
            second_divider = 11;
            break;
        case 19200:
            second_divider = 5;
            break;
        case 38400:
        case 153600:
            second_divider = 2;
            break;
        case 57600:
        case 230400:
        case 921600:
            second_divider = 1;
            break;
        default:
            break;
    }

    send_cmd_unchecked(CMD_SET_BAUD_RATE, first_divider, second_divider);

    m_baud_rate = baud_rate;
}

void UCamIII::set_light_freq(uint8_t light_freq) {
    send_cmd_unchecked(CMD_LIGHT, light_freq);

    switch (light_freq) {
        case FREQ_50:
            m_light_freq = 50;
            break;
        case FREQ_60:
            m_light_freq = 60;
            break;
        default:
            break;
    }
}

void UCamIII::set_tone(uint8_t contrast, uint8_t brightness, uint8_t exposure) {
    send_cmd_unchecked(CMD_SET_TONE, contrast, brightness, exposure);

    m_contrast = contrast;
    m_brightness = brightness;

    switch (exposure) {
        case TONE_MIN:
            m_exposure = -2;
            break;
        case TONE_LOW:
            m_exposure = -1;
            break;
        case TONE_NORMAL:
            m_exposure = 0;
            break;
        case TONE_HIGH:
            m_exposure = 1;
            break;
        case TONE_MAX:
            m_exposure = 2;
            break;
        default:
            break;
    }
}

void UCamIII::set_sleep_timeout(uint8_t timeout) {
    send_cmd_unchecked(CMD_SLEEP, timeout);

    m_sleep_timeout = timeout;
}

void UCamIII::snapshot(uint8_t snapshot_type, uint16_t skipped_frames) const {
    send_cmd(CMD_SNAPSHOT, SNAP_JPEG, (skipped_frames & 0xFF), (skipped_frames >> 8) & 0xFF);

    // Allow camera to finish writing to its buffer
    delay(500);
}

uint32_t UCamIII::get_picture(uint8_t picture_type) const {
    uint8_t data[NUM_CMD_BYTES];

    send_cmd(CMD_GET_PICTURE, picture_type);

    receive_cmd(data);
    if (data[0] != CMD_PREFIX || data[1] != CMD_DATA || data[2] != picture_type) {
        cerr << "Improper DATA response from GET PICTURE" << endl;
        return -1;
    }

    uint32_t img_size = 0x0 | ((uint32_t) data[5]) << 16;
    img_size |= ((uint32_t) data[4]) << 8;
    img_size |= ((uint32_t)  data[3]);

    return img_size;
}

void UCamIII::write_jpeg_data(uint32_t len) const {
    uint16_t num_pkgs = std::ceil((float) len / (((float) m_pkg_size - 6.0f)));
    uint16_t pkg_id, data_len, verify_code;
    uint32_t sum;
    uint8_t byte;
    uint8_t img_data[m_pkg_size - 6];

    send_cmd_unchecked(CMD_ACK);
    m_fout.open("img.jpeg");

    // Receive image data packages
    for (uint16_t i = 1; i <= num_pkgs; i++) {
        // Reset variables
        pkg_id = 0;
        data_len = 0;
        verify_code = 0;
        sum = 0;

        // Get first 2 bytes: package ID
        byte = serialGetchar((int) m_serial_port);
        sum += byte;
        pkg_id += byte;

        byte = serialGetchar((int) m_serial_port);
        sum += byte;
        pkg_id += byte << 8;

        if (pkg_id != i) {
            cerr << "Mismatched JPEG data packages" << endl;
            return;
        }

        // Get next 2 bytes: data length
        byte = serialGetchar((int) m_serial_port);
        sum += byte;
        data_len += byte;

        byte = serialGetchar((int) m_serial_port);
        sum += byte;
        data_len += byte << 8;

        // Collect image data
        for (int j = 0; j < data_len; j++) {
            byte = serialGetchar((int) m_serial_port);
            sum += byte;
            img_data[j] = byte;
        }

        // Get last 2 byte: verify code
        verify_code += serialGetchar((int) m_serial_port);
        verify_code += serialGetchar((int) m_serial_port) << 8;
        if (verify_code != (sum & 0xFF)) {
            cerr << "JPEG data package verification failed on package " << pkg_id << endl;
            return;
        }

        // Write image data
        m_fout.write((char*) img_data, data_len);
        cout << "Wrote package " << pkg_id << " with " << data_len << " bytes" << endl;

        // Package received successfully
        if (i == num_pkgs) {
            // Final package success
            send_cmd_unchecked(CMD_ACK, 0, 0, 0xF0, 0xF0);
        } else {
            send_cmd_unchecked(CMD_ACK, 0, 0, (i & 0xFF), (i >> 8));
        }
    }

    m_fout.close();
}

void UCamIII::write_raw_data(uint32_t len) const {
    char byte;
    uint32_t i = 0;

    m_fout.open("img.raw");

    while (i < len) {
        if (serialDataAvail((int) m_serial_port) > 0) {
            byte = (char) serialGetchar((int) m_serial_port);
            m_fout.write(&byte, 1);
            i++;
        } else {
            delay(5);
        }
    }

    // Data received successfully
    send_cmd_unchecked(CMD_ACK, CMD_DATA, 0, 1);
    m_fout.close();
}

void UCamIII::print_cmd(uint8_t *cmd, uint8_t len) {
    cout << cmd_to_str(cmd[1]) << " ";
    for (uint8_t i = 0; i < len; i++) {
        cout << std::setfill('0') << std::setw(2) << std::hex << (int) cmd[i] << " ";
    }
    cout << endl;
}

std::string UCamIII::parse_nak_err(uint8_t nak_err) {
    std::string msg = "UCam Error: NAK received: ";

    switch (nak_err) {
        case UCamIII::ERR_PICTURE_TYPE:
            msg += "Picture type error";
            break;
        case UCamIII::ERR_PICTURE_UP_SCALE:
            msg += "Picture up scale";
            break;
        case UCamIII::ERR_PICTURE_SCALE:
            msg += "Picture scale error";
            break;
        case UCamIII::ERR_UNEXPECTED_REPLY:
            msg += "Unexpected reply";
            break;
        case UCamIII::ERR_SEND_PICTURE_TIMEOUT:
            msg += "Send picture timeout";
            break;
        case UCamIII::ERR_UNEXPECTED_COMMAND:
            msg += "Unexpected command";
            break;
        case UCamIII::ERR_SRAM_JPEG_TYPE:
            msg += "SRAM JPEG type error";
            break;
        case UCamIII::ERR_SRAM_JPEG_SIZE:
            msg += "SRAM JPEG size error";
            break;
        case UCamIII::ERR_PICTURE_FORMAT:
            msg += "Picture format error";
            break;
        case UCamIII::ERR_PICTURE_SIZE:
            msg += "Picture size error";
            break;
        case UCamIII::ERR_PARAMETER:
            msg += "Parameter error";
            break;
        case UCamIII::ERR_SEND_REGISTER_TIMEOUT:
            msg += "Send register timeout";
            break;
        case UCamIII::ERR_COMMAND_ID:
            msg += "Command ID error";
            break;
        case UCamIII::ERR_PICTURE_NOT_READY:
            msg += "Picture not ready";
            break;
        case UCamIII::ERR_TRANSFER_PACKAGE_NUM:
            msg += "Transfer package number error";
            break;
        case UCamIII::ERR_SET_TRANSFER_PACKAGE_SIZE_WRONG:
            msg += "Set transfer package size wrong";
            break;
        case UCamIII::ERR_COMMAND_HEADER:
            msg += "Command header error";
            break;
        case UCamIII::ERR_COMMAND_LENGTH:
            msg += "Command length error";
            break;
        case UCamIII::ERR_SEND_PICTURE:
            msg += "Send picture error";
            break;
        case UCamIII::ERR_SEND_COMMAND:
            msg += "Send command error";
            break;
        default:
            msg += "Unknown error";
            break;
    }

    return msg;
}

std::string UCamIII::cmd_to_str(uint16_t cmd) {
    switch (cmd) {
        case CMD_INITIAL:
            return "INITIAL";
        case CMD_GET_PICTURE:
            return "GET PICTURE";
        case CMD_SNAPSHOT:
            return "SNAPSHOT";
        case CMD_SET_PACKAGE_SIZE:
            return "SET PKG SIZE";
        case CMD_SET_BAUD_RATE:
            return "SET BAUD RATE";
        case CMD_RESET:
            return "RESET";
        case CMD_DATA:
            return "DATA";
        case CMD_SYNC:
            return "SYNC";
        case CMD_ACK:
            return "ACK";
        case CMD_NAK:
            return "NAK";
        case CMD_LIGHT:
            return "LIGHT";
        case CMD_SET_TONE:
            return "SET TONE";
        case CMD_SLEEP:
            return "SLEEP";
        default:
            return "";
    }
}
