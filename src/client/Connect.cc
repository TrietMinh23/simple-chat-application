#include "client.h"

int Socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        throw std::runtime_error("Socket creation failed");
    }
    return sockfd;
}

void Connect(int sockfd, const std::string &host, int port)
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0)
    {
        throw std::runtime_error("Invalid address/ Address not supported");
    }

    if (connect(sockfd, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Connection Failed");
    }
}
