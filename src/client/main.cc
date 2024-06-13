#include "client.h"

int main(int argc, char *argv[])
{
    std::string host = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? std::atoi(argv[2]) : 8080;

    try
    {
        int sockfd = Socket();
        Connect(sockfd, host, port);
        clientChat(sockfd);
        close(sockfd);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
