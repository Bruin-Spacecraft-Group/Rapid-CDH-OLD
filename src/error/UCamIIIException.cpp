#include "../sensors/UCamIII.h"
#include "UCamIIIException.h"

UCamIIIException::UCamIIIException(const string &msg)
    : std::runtime_error("UCam Error: " + msg) {}

UCamIIIException::UCamIIIException(int nak_err)
    : std::runtime_error(parse_nak_err(nak_err)) {}

std::string UCamIIIException::parse_nak_err(int nak_err) {
    string msg = "UCam Error: NAK received: ";

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