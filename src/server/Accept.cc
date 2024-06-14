#include "server.h"

int Accept(int sockfd)
{
    sockaddr_in clientAddr{};
    socklen_t addrSize = sizeof(clientAddr);
    int clientSock = accept(sockfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &addrSize);
    return clientSock;
}
