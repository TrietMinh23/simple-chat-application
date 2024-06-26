#include "./client.h"

int clientChat(int sockfd, const std::string &username)
{
    char buffer[BUFFER_SIZE] = {0};
    fd_set master;
    int maxfd = sockfd + 1;
    std::string activeChatroom;
    std::set<std::string> activeChatrooms;
    bool allowMessages = false;
    bool waitingForSelfResponse = false;
    struct timespec send_time, receive_time;

    while (true)
    {
        msgPrompt(username, activeChatroom);
        FD_ZERO(&master);
        FD_SET(sockfd, &master);
        FD_SET(STDIN_FILENO, &master);
        int sockCount = select(maxfd, &master, NULL, NULL, NULL);

        if (sockCount == -1)
        {
            throw std::runtime_error("socket count = -1");
        }
        else if (sockCount == 0)
        {
            cout << "select Timeout." << endl;
            continue;
        }

        if (FD_ISSET(STDIN_FILENO, &master))
        {
            std::string msgToSend;
            getline(cin, msgToSend);
            if (cin.fail() || cin.eof())
            {
                cout << "Connection closed!!\n";
                return 0;
            }
            if (msgToSend.empty())
                continue;

            auto cmdType = getCommandType(msgToSend);
            if (cmdType == cmd::NOT_CMD)
            {
                if (!allowMessages)
                {
                    printMessage(
                        "ERROR" + DELIM + "JOIN" + DELIM +
                        "You must join a chatroom first.");
                    continue;
                }
                // not a command, send as message
                auto chatroom = activeChatroom;
                if (msgToSend[0] == '#')
                {
                    auto pos = msgToSend.find(" ");
                    chatroom = msgToSend.substr(1, pos - 1);
                    msgToSend = msgToSend.substr(pos + 1);
                }
                std::string msg = chatroom + DELIM + msgToSend;
                clock_gettime(CLOCK_MONOTONIC, &send_time);
                send(sockfd, msg.c_str(), msg.size(), 0);
                continue;
            }

            switch (cmdType)
            {
            case cmd::QUIT:
                return 0;

            case cmd::HELP:
            {
                printHelpMessage();
                continue;
            }

            case cmd::JOIN:
            {
                auto chatroom = split(msgToSend, " ", 2)[1].substr(1);
                if (activeChatrooms.count(chatroom) != 0)
                {
                    // if client has already joined the asked chatroom
                    activeChatroom = chatroom;
                    continue;
                }
                std::string request = DELIM + "JOIN" + DELIM + chatroom;
                send(sockfd, request.c_str(), request.size(), 0);
                continue;
            }

            case cmd::LEAVE:
            {
                auto chatroom = split(msgToSend, " ", 2)[1].substr(1);
                std::string request = DELIM + "LEAVE" + DELIM + chatroom;
                send(sockfd, request.c_str(), request.size(), 0);
                continue;
            }

            case cmd::LIST_CHATROOMS:
            {
                std::string request = DELIM + "LIST_CHATROOMS";
                send(sockfd, request.c_str(), request.size(), 0);
                continue;
            }

            case cmd::LIST_PEOPLE:
            {
                if (!allowMessages)
                {
                    printMessage(
                        "ERROR" + DELIM + "JOIN" + DELIM +
                        "You must join a chatroom first.");
                    continue;
                }
                std::string request = DELIM + "LIST_PEOPLE" + DELIM + activeChatroom;
                send(sockfd, request.c_str(), request.size(), 0);
                continue;
            }

            case cmd::SELF:
            {
                std::string request = DELIM + "SELF";
                clock_gettime(CLOCK_MONOTONIC, &send_time);
                write(sockfd, request.c_str(), request.size());
                waitingForSelfResponse = true;
                continue;
            }

            case cmd::INVALID:
            default:
                printMessage("ERROR" + DELIM + "CMD" + DELIM + "Invalid command.");
            }
        }

        if (FD_ISSET(sockfd, &master))
        {
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0)
            {
                cout << "Server unavailable\n";
                return 0;
            }

            clock_gettime(CLOCK_MONOTONIC, &receive_time);
            std::string receivedMsg(buffer);

            if (receivedMsg.find(DELIM) == 0)
            {
                // process response of request
                auto tokens = split(receivedMsg, DELIM, 2);
                auto &type = tokens[0], &arg = tokens[1];
                if (waitingForSelfResponse && type == "SELF_RESPONSE")
                {
                    waitingForSelfResponse = false;
                    auto round_trip_time = (receive_time.tv_sec - send_time.tv_sec) * 1000.0 +
                                           (receive_time.tv_nsec - send_time.tv_nsec) / 1000000.0;
                    auto connection_time = round_trip_time / 2;
                    cout << "Round-trip time: " << round_trip_time << " ms. Connection time: " << connection_time << " ms.\n";
                }
                else if (type == "JOIN")
                {
                    allowMessages = true;
                    activeChatroom = arg;
                    activeChatrooms.insert(arg);
                }
                else if (type == "LEAVE")
                {
                    printMessage("INFO" + DELIM + "#" + arg + DELIM + "you left.");
                    activeChatrooms.erase(arg);
                    if (activeChatrooms.size() == 0)
                    {
                        activeChatroom = "";
                        allowMessages = false;
                    }
                    else
                    {
                        activeChatroom = *activeChatrooms.begin();
                    }
                }
                continue;
            }
            printMessage(receivedMsg);
        }
    }

    return 0;
}
