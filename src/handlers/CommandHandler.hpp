#ifndef RAPID_CDH_COMMAND_HANDLER_HPP
#define RAPID_CDH_COMMAND_HANDLER_HPP

#include <cstdint>
#include <map>

#include "../globals.h"
#include "../util/Result.hpp"

/** Struct defining a Command */
struct Command {
    /** Unique command id */
    uint8_t cmd_id;
    /** Unique subsystem id */
    uint8_t subsystem_id;
    /** Type of command */
    uint8_t cmd_type;
    /** Length of command */
    uint32_t params_len;
    /** Command parameters */
    uint8_t *params;
    /** Time to execute command (ms) */
    uint64_t exec_time;
    /** Timeout (s) */
    uint32_t timeout;
    /** Checksum of entire command */
    uint32_t checksum;
};

/** Struct defining a Reponse */
struct Response {
    /** Unique command id */
    uint8_t cmd_id;
    /** Unique subsystem id */
    uint8_t subsystem_id;
    /** Status of command execution */
    uint8_t status;
    /** Length of returned data */
    uint32_t data_len;
    /** Returned data */
    uint8_t *data;
    /** Checksum of entire response */
    uint32_t checksum;
};

/** Handles command scheduling */
class CommandHandler {
public:
    CommandHandler(uint32_t max_threads);

    void add_command(Command cmd);
    void remove_command(uint8_t cmd_id);

private:
    /** Thread pool size */
    uint32_t m_max_threads;
    /** Commands */
    std::map<uint8_t, Command> m_cmds;
};


#endif // RAPID_CDH_COMMAND_HANDLER_H
