#ifndef RAPIDCDH_UCAM_III_H
#define RAPIDCDH_UCAM_III_H

#include <cstdint>

class UCamIII {
public:
    UCamIII(const char* serial_dev, const int baud_rate, const int res_pin);
    ~UCamIII();

    void reset() const;

    // Enums
    enum class CmdID {
        INITIAL          = 0x01,
        GET_PICTURE      = 0x04,
        SNAPSHOT         = 0x05,
        SET_PACKAGE_SIZE = 0x06,
        SET_BAUD_RATE    = 0x07,
        RESET            = 0x08,
        DATA             = 0x0A,
        SYNC             = 0x0D,
        ACK              = 0x0E,
        NAK              = 0x0F,
        LIGHT            = 0x13,
        SET_TONE         = 0x14, // Contrast, brightness, exposure
        SLEEP            = 0x15
    };

    enum class ImgFormat {
        RAW_GRAY_8    = 0x03, // 8-bit Gray Scale (RAW, 8-bit for Y only)
        RAW_CrYCbY_16 = 0x08, // 16-bit Colour (RAW, CrYCbY)
        RAW_RGB_16    = 0x06, // 16-bit Colour (RAW, 565(RGB))
        JPEG          = 0x07
    };

    enum class RawRes {
        RES_60x80   = 0x01,
        RES_160x120 = 0x03,
        RES_128x128 = 0x09,
        RES_128x96  = 0x0B
    };

    enum class JPEGRes {
        RES_160x128 = 0x03,
        RES_320x240 = 0x05,
        RES_640x480 = 0x07
    };

    enum class PictureType {
        SNAPSHOT = 0x01,
        RAW      = 0x02,
        JPEG     = 0x05
    };

    enum class SnapshotType {
        JPEG = 0x00, // Compressed
        RAW  = 0x01  // Uncompressed
    };

    enum class ResetType {
        ALL,            // Resets the whole system. The uCAM-III will reboot and reset all registers and state machines.
        STATE_MACHINES  // Resets the state machines only
    };

    enum class Error {
        NO_ERR,
        PICTURE_TYPE_ERR,
        PICTURE_UP_SCALE,
        PICTURE_SCALE_ERR,
        UNEXPECTED_REPLY,
        SEND_PICTURE_TIMEOUT,
        UNEXPECTED_COMMAND,
        SRAM_JPEG_TYPE_ERR,
        SRAM_JPEG_SIZE_ERR,
        PICTURE_FORMAT_ERR,
        PICTURE_SIZE_ERR,
        PARAMETER_ERR,
        SEND_REGISTER_TIMEOUT,
        COMMAND_ID_ERR,
        PICTURE_NOT_READY,
        TRANSFER_PACKAGE_NUM_ERR,
        SET_TRANSFER_PACKAGE_SIZE_WRONG,
        COMMAND_HEADER_ERR,
        COMMAND_LENGTH_ERR,
        SEND_PICTURE_ERR,
        SEND_COMMAND_ERR
    };

    enum class LightFreqType {
        HZ_50,
        HZ_60
    };

    // For brightness, contrast, exposure
    enum class Tone {
        MIN,    // Exposure: -2
        LESS,    // Exposure: -1
        NORMAL, // Default. Exposure: 0
        MORE,   // Exposure: +1
        MAX     // Exposure: +2
    };

private:
    int serial_port;
    int res_pin;

    void send_cmd(CmdID cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) const;
};

#endif //RAPIDCDH_UCAM_III_H