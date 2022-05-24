#ifndef RAPIDCDH_COMMANDSCHEDULER_H
#define RAPIDCDH_COMMANDSCHEDULER_H

#include <cstdint>
#include <queue>
#include <unordered_map>

#include "../globals.h"

struct Command {
    uint8_t cmd_id; // Unique id
    uint8_t subsystem_id;
    uint8_t cmd_num;
    uint16_t params_len;
    uint8_t *params;
    uint64_t exec_time; // Time to execute command (ms)
    uint16_t timeout; // (s)
    uint32_t checksum;
};

struct Response {
    uint8_t subsystem_id;
    uint8_t cmd_id;
    uint8_t status;
    uint16_t data_len;
    uint8_t *data;
    uint32_t checksum;
};

class CommandScheduler {
public:
    CommandScheduler();

    Status add_command(Command cmd);
    Status remove_command(uint8_t cmd_id);

private:
    std::queue<Command> cmd_queue;
    std::unordered_map<uint8_t, Command> cmd_map;
};


#endif //RAPIDCDH_COMMANDSCHEDULER_H
