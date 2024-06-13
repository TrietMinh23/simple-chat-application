#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Namespace for command types
namespace cmd
{
    enum Commands
    {
        NOT_CMD,
        JOIN,
        LEAVE,
        LIST_CHATROOMS,
        LIST_PEOPLE
    };
}

// Utility constants
const int BUFFER_SIZE = 1024;
const std::string DELIM("$$$");

// Server utility functions
int Socket();
void Bind(int sockfd, int port);
void Listen(int sockfd, int backlog);
int Accept(int sockfd);

void serverChat(int clientSockfd);

std::vector<std::string> split(const std::string &str, const std::string &delim);

#endif // SERVER_H_
