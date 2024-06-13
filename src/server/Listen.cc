#include "server.h"

void Listen(int sockfd, int backlog)
{
    if (listen(sockfd, backlog) == -1)
    {
        throw std::runtime_error("Listen failed");
    }
}