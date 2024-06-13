#include "server.h"
#include <sstream>
#include <unordered_map>
#include <set>
#include <mutex>
#include <chrono>

std::unordered_map<std::string, std::set<int>> chatRooms;
std::unordered_map<int, std::chrono::steady_clock::time_point> clientConnectionTimes;
std::mutex chatRoomsMutex;

void broadcastMessage(int senderSockfd, const std::string &roomName, const std::string &message)
{
    std::lock_guard<std::mutex> lock(chatRoomsMutex);
    if (chatRooms.find(roomName) != chatRooms.end())
    {
        for (int fd : chatRooms[roomName])
        {
            if (fd != senderSockfd)
            {
                send(fd, message.c_str(), message.length(), 0);
            }
        }
    }
}

void handleJoin(int sockfd, const std::string &roomName)
{
    std::lock_guard<std::mutex> lock(chatRoomsMutex);
    chatRooms[roomName].insert(sockfd);
    clientConnectionTimes[sockfd] = std::chrono::steady_clock::now(); // Start time for connection
    std::string joinMsg = "Server: User has joined the room.\n";
    broadcastMessage(sockfd, roomName, joinMsg);
}

void handleLeave(int sockfd, const std::string &roomName)
{
    std::lock_guard<std::mutex> lock(chatRoomsMutex);
    if (chatRooms.find(roomName) != chatRooms.end())
    {
        chatRooms[roomName].erase(sockfd);
        if (chatRooms[roomName].empty())
        {
            chatRooms.erase(roomName);
        }
        auto endTime = std::chrono::steady_clock::now();
        auto startTime = clientConnectionTimes[sockfd];
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        std::string leaveMsg = "Server: User has left the room. Connection time: " + std::to_string(duration) + " seconds.\n";
        broadcastMessage(sockfd, roomName, leaveMsg);
        clientConnectionTimes.erase(sockfd); // Remove the start time record
    }
}

void handleUptime(int sockfd)
{
    std::lock_guard<std::mutex> lock(chatRoomsMutex);
    auto currentTime = std::chrono::steady_clock::now();
    for (const auto &pair : clientConnectionTimes)
    {
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - pair.second).count();
        std::string uptimeMsg = "Client " + std::to_string(pair.first) + " has been connected for " + std::to_string(duration) + " seconds.\n";
        send(sockfd, uptimeMsg.c_str(), uptimeMsg.length(), 0);
    }
}

void handleListChatrooms(int sockfd)
{
    std::string response = "Available Chat Rooms:\n";
    std::lock_guard<std::mutex> lock(chatRoomsMutex);
    for (auto &room : chatRooms)
    {
        response += room.first + " (" + std::to_string(room.second.size()) + " participants)\n";
    }
    send(sockfd, response.c_str(), response.length(), 0);
}

void handleMessage(int sockfd, const std::string &roomName, const std::string &message)
{
    std::string formattedMessage = "Message: " + message + "\n";
    broadcastMessage(sockfd, roomName, formattedMessage);
}

void serverChat(int sockfd)
{
    char buffer[BUFFER_SIZE];
    std::string currentRoom;

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesRead = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
        {
            std::cout << "Client disconnected or error occurred" << std::endl;
            if (!currentRoom.empty())
            {
                handleLeave(sockfd, currentRoom);
            }
            break;
        }
        else if (bytesRead > 0)
        {
            std::cout << "Received: " << std::string(buffer, bytesRead) << std::endl;
        }

        std::string msg(buffer, bytesRead);
        std::istringstream iss(msg);
        std::string command, param;
        iss >> command;

        std::cout << "Command: " << command << std::endl;


        if (command == "/join")
        {
            iss >> param;
            if (!currentRoom.empty())
            {
                handleLeave(sockfd, currentRoom);
            }
            currentRoom = param;
            handleJoin(sockfd, currentRoom);
        }
        else if (command == "/leave")
        {
            if (!currentRoom.empty())
            {
                handleLeave(sockfd, currentRoom);
                currentRoom.clear();
            }
        }
        else if (command == "/list")
        {
            handleListChatrooms(sockfd);
        }
        else if (command == "/uptime")
        {
            handleUptime(sockfd);
        }
        else
        {
            if (!currentRoom.empty())
            {
                handleMessage(sockfd, currentRoom, msg);
            }
        }
    }
}
