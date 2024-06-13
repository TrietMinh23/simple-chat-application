#include "server.h"

int main(int argc, char *argv[])
{
    const int port = (argc > 1) ? std::atoi(argv[1]) : 8080;
    int sockfd = Socket();
    Bind(sockfd, port);
    Listen(sockfd, 5);

    std::vector<std::thread> clientThreads;

    while (true)
    {
        int clientSock = Accept(sockfd);
        if (clientSock < 0)
        {
            std::cerr << "Failed to accept client." << std::endl;
            continue;
        }
        clientThreads.emplace_back([clientSock]
                                   {
            serverChat(clientSock);
            close(clientSock); });
    }

    // Join all threads (not reachable in this simple example)
    for (auto &th : clientThreads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    close(sockfd);
    return 0;
}
