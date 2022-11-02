#include <algorithm>

#include "CommandHandler.hpp"

CommandHandler::CommandHandler(uint32_t max_threads)
    : m_max_threads(max_threads) {
    m_cmds = std::map<uint8_t, Command>();
    // TODO: initialize thread pool
}

void CommandHandler::add_command(Command cmd) {
    m_cmds[cmd.cmd_id] = cmd;
}

void CommandHandler::remove_command(uint8_t cmd_id) {
    if (m_cmds.contains(cmd_id))
        m_cmds.erase(cmd_id);
}