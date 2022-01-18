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

Status UCamIII::init() {
    pinMode(m_rst_pin, OUTPUT);
    digitalWrite(m_rst_pin, HIGH);

    // Open serial connection
    if ((m_serial_port = serialOpen(m_serial_dev, (int) m_baud_rate)) < 0) {
        cerr << "Unable to open serial device: " << std::string(m_serial_dev) << endl;
        return FAILURE;
    }

    // Synchronization
    hard_reset();
    Status status = sync();
    if (!status) return status;

    // Allow AGC and AEC circuits to stabilize
    delay(2000);

    // Configuration
    return send_cmd(CMD_INITIAL, 0x00, m_img_format, m_resolution, m_resolution);
}

Status UCamIII::sync() const {
    uint8_t data[NUM_CMD_BYTES];
    Status status;

    // Send SYNC command until ACK command is received
    for (uint8_t i = 0; i < MAX_TRIES; i++) {
        // Send SYNC command
        send_cmd_unchecked(CMD_SYNC);

        // Check if ACK command received
        status = receive_cmd(data, NUM_CMD_BYTES, 5 + i);
        if (!status) return status;

        if (data[0] == CMD_PREFIX && data[1] == CMD_ACK && data[2] == CMD_SYNC) {
            // Check if SYNC command received
            status = receive_cmd(data, NUM_CMD_BYTES, 5 + i);
            if (!status) return status;

            if (data[0] == CMD_PREFIX && data[1] == CMD_SYNC) {
                delay(10);
                // Respond with ACK command
                send_cmd_unchecked(CMD_ACK, CMD_SYNC);

                if (constants::DEBUG) {
                    cout << "Synchronization completed" << endl;
                }

                return SUCCESS;
            }
        }
    }

    cerr << "Synchronization failed after " << MAX_TRIES << " tries" << endl;
    return FAILURE;
}

void UCamIII::hard_reset() const {
    digitalWrite(m_rst_pin, LOW);
    delay(10);
    digitalWrite(m_rst_pin, HIGH);
    delay(10);
}

Status UCamIII::soft_reset(ResetType rst_type, bool immediate) const {
    if (immediate) {
        return send_cmd(CMD_RESET, rst_type, 0x00, 0x00, 0xFF);
    }
    else {
        return send_cmd(CMD_RESET, rst_type);
    }
}

void UCamIII::send_cmd_unchecked(CmdID cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    uint8_t data[NUM_CMD_BYTES] = {CMD_PREFIX, cmd, param1, param2, param3, param4};
    serialPuts((int) m_serial_port, (char*) data);

    if (constants::DEBUG) {
        cout << "UCam Sent: ";
        print_cmd(data);
    }
}

Status UCamIII::send_cmd(CmdID cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const {
    send_cmd_unchecked(cmd, param1, param2, param3, param4);

    // Check response
    uint8_t data[NUM_CMD_BYTES];
    Status status = receive_cmd(data);
    if (!status) return status;

    if (data[0] == CMD_PREFIX && data[1] == CMD_NAK) {
        cerr << parse_nak_err((Error) data[4]) << endl;
        return FAILURE;
    }
    else if (data[0] != CMD_PREFIX || data[1] != CMD_ACK || data[2] != cmd) {
        cerr << "Command verification not received for " << cmd_to_str(cmd) << endl;
        return FAILURE;
    }

    return SUCCESS;
}

Status UCamIII::receive_cmd(uint8_t* data, uint8_t len, uint16_t timeout) const {
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
        return FAILURE;
    }
    else {
        if (constants::DEBUG) {
            cout << "UCam Received " << (int) i << " bytes: ";
            print_cmd(data, i);
        }

        return SUCCESS;
    }
}

Status UCamIII::initial(ImgFormat img_format, Resolution resolution) {
    Status status = send_cmd(CMD_INITIAL, 0x00, img_format, resolution, resolution);
    if (!status) return status;

    m_img_format = img_format;
    m_resolution = resolution;

    return SUCCESS;
}

Status UCamIII::set_package_size(uint32_t size) {
    Status status = send_cmd(CMD_SET_PACKAGE_SIZE, 0x08, (size & 0xFF), (size >> 8) & 0xFF);
    if (!status) return status;

    if (size > MAX_PKG_SIZE) {
        m_pkg_size = MAX_PKG_SIZE;
    }
    else {
        m_pkg_size = size;
    }

    return SUCCESS;
}

Status UCamIII::set_baud_rate(uint32_t baud_rate) {
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
            return FAILURE;
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

    return SUCCESS;
}

void UCamIII::set_light_freq(LightFreq light_freq) {
    send_cmd_unchecked(CMD_LIGHT, light_freq);

    switch (light_freq) {
        case FREQ_50:
            m_light_freq = 50;
            break;
        case FREQ_60:
            m_light_freq = 60;
            break;
    }
}

void UCamIII::set_tone(Tone contrast, Tone brightness, Tone exposure) {
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
    }
}

void UCamIII::set_sleep_timeout(uint8_t timeout) {
    send_cmd_unchecked(CMD_SLEEP, timeout);

    m_sleep_timeout = timeout;
}

Status UCamIII::snapshot(SnapshotType snapshot_type, uint16_t skipped_frames) const {
    Status status = send_cmd(CMD_SNAPSHOT, SNAP_JPEG, (skipped_frames & 0xFF), (skipped_frames >> 8) & 0xFF);
    if (!status) return status;

    // Allow camera to finish writing to its buffer
    delay(500);

    return SUCCESS;
}

Status UCamIII::get_picture(PictureType picture_type, uint32_t& len) const {
    uint8_t data[NUM_CMD_BYTES];

    Status status = send_cmd(CMD_GET_PICTURE, picture_type);
    if (!status) return status;

    status = receive_cmd(data);
    if (!status) return status;

    if (data[0] != CMD_PREFIX || data[1] != CMD_DATA || data[2] != picture_type) {
        cerr << "Improper DATA response from GET PICTURE" << endl;
        return FAILURE;
    }

    len = 0x0 | ((uint32_t) data[5]) << 16;
    len |= ((uint32_t) data[4]) << 8;
    len |= ((uint32_t)  data[3]);

    return SUCCESS;
}

Status UCamIII::write_jpeg_data(uint32_t len) const {
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
            return FAILURE;
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
            return FAILURE;
        }

        // Write image data
        m_fout.write((char*) img_data, data_len);

        if (constants::DEBUG) {
            cout << "Wrote package " << pkg_id << " with " << data_len << " bytes" << endl;
        }

        // Package received successfully
        if (i == num_pkgs) {
            // Final package success
            send_cmd_unchecked(CMD_ACK, 0, 0, 0xF0, 0xF0);
        }
        else {
            send_cmd_unchecked(CMD_ACK, 0, 0, (i & 0xFF), (i >> 8));
        }
    }

    m_fout.close();
    return SUCCESS;
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
        }
        else {
            delay(5);
        }
    }

    // Data received successfully
    send_cmd_unchecked(CMD_ACK, CMD_DATA, 0, 1);
    m_fout.close();
}

void UCamIII::print_cmd(uint8_t *cmd, uint8_t len) {
    cout << cmd_to_str((CmdID) cmd[1]) << " ";
    for (uint8_t i = 0; i < len; i++) {
        cout << std::setfill('0') << std::setw(2) << std::hex << (int) cmd[i] << " ";
    }
    cout << endl;
}

std::string UCamIII::parse_nak_err(Error nak_err) {
    std::string msg = "UCam Error: NAK received: ";

    switch (nak_err) {
        case ERR_NONE:
            msg += "No error";
            break;
        case ERR_PICTURE_TYPE:
            msg += "Picture type error";
            break;
        case ERR_PICTURE_UP_SCALE:
            msg += "Picture up scale";
            break;
        case ERR_PICTURE_SCALE:
            msg += "Picture scale error";
            break;
        case ERR_UNEXPECTED_REPLY:
            msg += "Unexpected reply";
            break;
        case ERR_SEND_PICTURE_TIMEOUT:
            msg += "Send picture timeout";
            break;
        case ERR_UNEXPECTED_COMMAND:
            msg += "Unexpected command";
            break;
        case ERR_SRAM_JPEG_TYPE:
            msg += "SRAM JPEG type error";
            break;
        case ERR_SRAM_JPEG_SIZE:
            msg += "SRAM JPEG size error";
            break;
        case ERR_PICTURE_FORMAT:
            msg += "Picture format error";
            break;
        case ERR_PICTURE_SIZE:
            msg += "Picture size error";
            break;
        case ERR_PARAMETER:
            msg += "Parameter error";
            break;
        case ERR_SEND_REGISTER_TIMEOUT:
            msg += "Send register timeout";
            break;
        case ERR_COMMAND_ID:
            msg += "Command ID error";
            break;
        case ERR_PICTURE_NOT_READY:
            msg += "Picture not ready";
            break;
        case ERR_TRANSFER_PACKAGE_NUM:
            msg += "Transfer package number error";
            break;
        case ERR_SET_TRANSFER_PACKAGE_SIZE_WRONG:
            msg += "Set transfer package size wrong";
            break;
        case ERR_COMMAND_HEADER:
            msg += "Command header error";
            break;
        case ERR_COMMAND_LENGTH:
            msg += "Command length error";
            break;
        case ERR_SEND_PICTURE:
            msg += "Send picture error";
            break;
        case ERR_SEND_COMMAND:
            msg += "Send command error";
            break;
    }

    return msg;
}

std::string UCamIII::cmd_to_str(CmdID cmd) {
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
