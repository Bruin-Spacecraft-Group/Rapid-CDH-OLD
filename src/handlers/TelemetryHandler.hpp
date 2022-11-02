#ifndef RAPID_CDH_TELEMETRY_HANDLER_HPP
#define RAPID_CDH_TELEMETRY_HANDLER_HPP

#include <cstdint>

/** Struct defining telemetry packet */
typedef struct Packet {
    /** Unique packet id */
    uint8_t packet_id;
    /** Unique subsystem id */
    uint8_t subsystem_id;
    /** Length of data */
    uint32_t data_len;
    /** Packet data */
    uint8_t *data;
    /** Checksum of entire packet */
    uint32_t checksum;
} Packet;

/** Handles encoding/decoding telemetry packets, prioritizing packets to send */
class TelemetryHandler {
public:
    TelemetryHandler();

private:
};

#endif // RAPID_CDH_TELEMETRY_HANDLER_HPP
