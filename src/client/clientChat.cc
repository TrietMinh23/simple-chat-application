#include "client.h"

void clientChat(int sockfd)
{
    std::string input;
    std::cout << "Connected to the server. Type messages or commands:\n";

    while (true)
    {
        std::getline(std::cin, input);
        if (input == "/exit")
            break; // Command to exit client

        send(sockfd, input.c_str(), input.length(), 0);

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0)
        {
            std::cout << "Server: " << std::string(buffer, bytesReceived) << std::endl;
        }
    }
}
