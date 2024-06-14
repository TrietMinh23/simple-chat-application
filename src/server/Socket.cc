#include "server.h"

int Socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        throw std::runtime_error("Socket creation failed");
    }
    return sockfd;
}