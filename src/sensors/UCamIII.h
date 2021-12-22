#ifndef RAPIDCDH_UCAM_III_H
#define RAPIDCDH_UCAM_III_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

class UCamIII {
public:
    UCamIII(const char* serial_dev, int baud_rate, int rst_pin,
            uint8_t img_format, uint8_t raw_res, uint8_t jpeg_res, std::ofstream& fout);
    ~UCamIII();

    void init();
    void hardware_reset() const;

    void send_cmd_unchecked(int cmd, uint8_t param1 = 0, uint8_t param2 = 0, uint8_t param3 = 0, uint8_t param4 = 0) const;
    void send_cmd(int cmd, uint8_t param1 = 0, uint8_t param2 = 0, uint8_t param3 = 0, uint8_t param4 = 0) const;
    void receive_cmd(int* data) const;

    void sync() const;
    void initial(uint8_t img_format, uint8_t raw_res, uint8_t jpeg_res);
    void set_package_size(int size);
    void snapshot(uint8_t snapshot_type, int skipped_frames = 0) const;
    [[nodiscard]] int get_picture(uint8_t picture_type) const;

    void write_jpeg_data(int len) const;
    void write_raw_data(int len) const;

    // Enums
    enum CmdID {
        CMD_INITIAL          = 0x01,
        CMD_GET_PICTURE      = 0x04,
        CMD_SNAPSHOT         = 0x05,
        CMD_SET_PACKAGE_SIZE = 0x06,
        CMD_SET_BAUD_RATE    = 0x07,
        CMD_RESET            = 0x08,
        CMD_DATA             = 0x0A,
        CMD_SYNC             = 0x0D,
        CMD_ACK              = 0x0E,
        CMD_NAK              = 0x0F,
        CMD_LIGHT            = 0x13,
        CMD_SET_TONE         = 0x14, // Contrast, brightness, exposure
        CMD_SLEEP            = 0x15
    };

    enum ImgFormat {
        FMT_RAW_GRAY_8    = 0x03, // 8-bit Gray Scale (RAW, 8-bit for Y only)
        FMT_RAW_CRYCBY_16 = 0x08, // 16-bit Colour (RAW, CrYCbY)
        FMT_RAW_RGB_16    = 0x06, // 16-bit Colour (RAW, 565(RGB))
        FMT_JPEG          = 0x07
    };

    enum RawRes {
        RAW_60x80   = 0x01,
        RAW_160x120 = 0x03,
        RAW_128x128 = 0x09,
        RAW_128x96  = 0x0B
    };

    enum JPEGRes {
        JPEG_160x128 = 0x03,
        JPEG_320x240 = 0x05,
        JPEG_640x480 = 0x07
    };

    enum PictureType {
        PIC_SNAPSHOT = 0x01,
        PIC_RAW      = 0x02,
        PIC_JPEG     = 0x05
    };

    enum SnapshotType {
        SNAP_JPEG, // Compressed
        SNAP_RAW   // Uncompressed
    };

    enum ResetType {
        RST_ALL,            // Resets the whole system. The uCAM-III will reboot and reset all registers and state machines.
        RST_STATE_MACHINES  // Resets the state machines only
    };

    enum Error {
        ERR_NONE,
        ERR_PICTURE_TYPE,
        ERR_PICTURE_UP_SCALE,
        ERR_PICTURE_SCALE,
        ERR_UNEXPECTED_REPLY,
        ERR_SEND_PICTURE_TIMEOUT,
        ERR_UNEXPECTED_COMMAND,
        ERR_SRAM_JPEG_TYPE,
        ERR_SRAM_JPEG_SIZE,
        ERR_PICTURE_FORMAT,
        ERR_PICTURE_SIZE,
        ERR_PARAMETER,
        ERR_SEND_REGISTER_TIMEOUT,
        ERR_COMMAND_ID,
        ERR_PICTURE_NOT_READY,
        ERR_TRANSFER_PACKAGE_NUM,
        ERR_SET_TRANSFER_PACKAGE_SIZE_WRONG,
        ERR_COMMAND_HEADER,
        ERR_COMMAND_LENGTH,
        ERR_SEND_PICTURE,
        ERR_SEND_COMMAND
    };

    enum LightFreqType { // Hz
        FREQ_50,
        FREQ_60
    };

    // For brightness, contrast, exposure
    enum Tone {
        TONE_MIN,    // Exposure: -2
        TONE_LOW,    // Exposure: -1
        TONE_NORMAL, // Default. Exposure: 0
        TONE_HIGH,   // Exposure: +1
        TONE_MAX     // Exposure: +2
    };

private:
    const char* m_serial_dev;
    int m_baud_rate;
    int m_serial_port;
    int m_rst_pin;

    int m_pkg_size;
    uint8_t m_img_format;
    uint8_t m_raw_res;
    uint8_t m_jpeg_res;

    std::ofstream& m_fout;

    // UCam parameters
    enum Params {
        NUM_CMD_BYTES = 6,     // Number of bytes in one command
        CMD_PREFIX    = 0xAA,  // First byte of all commands
        MAX_PKG_SIZE  = 512,   // Bytes
        MAX_TRIES     = 60     // Max number of tries for SYNC during synchronization
    };
};

#endif //RAPIDCDH_UCAM_III_H