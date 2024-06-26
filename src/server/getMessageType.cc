#include "./server.h"

cmd::Commands getMessageType(const std::string &msg)
{
    using namespace cmd;
    if (msg.find(DELIM) != 0)
        return NOT_CMD;

    std::string cmd(msg.substr(DELIM.size()));
    if (cmd.find("JOIN") == 0)
        return JOIN;

    if (cmd.find("LEAVE") == 0)
        return LEAVE;

    if (cmd.find("LIST_CHATROOMS") == 0)
        return LIST_CHATROOMS;

    if (cmd.find("LIST_PEOPLE") == 0)
        return LIST_PEOPLE;

    if (cmd.find("SELF") == 0)
        return SELF;

    if (cmd.find("ALL") == 0)
        return ALL;

    return INVALID;
}
