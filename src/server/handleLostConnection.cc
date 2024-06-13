#include "./server.h"

void handleLostConnection(const int currentClientFd, ChatroomToFdList &chatRooms, FdToName &clients, Database<User> &db)
{
    // Lock the resources with a mutex if necessary to prevent race conditions
    // std::lock_guard<std::mutex> lock(resourceMutex); // Uncomment and define mutex if shared resources are accessed

    // Log that the client disconnected
    cout << color::magenta << "[CLIENT:OFFLINE] " << color::reset
         << "<" << clients[currentClientFd] << "> went offline" << endl;

    // Broadcast a user has gone offline message to all chat rooms they were part of
    std::string username = clients[currentClientFd]; // Retrieve the username from the map
    std::string message = "INFO" + DELIM + username + DELIM + "went offline";
    for (auto &chatRoom : chatRooms)
    {
        if (chatRoom.second.find(currentClientFd) != chatRoom.second.end())
        {
            chatRoom.second.erase(currentClientFd);               // Remove the client from the chat room
            broadcast(chatRoom.second, currentClientFd, message); // Send offline notification
        }
    }

    // Remove the client from the global client list
    clients.erase(currentClientFd); // Erase client from the map of client names
    close(currentClientFd);         // Close the client's socket

    // Update the user's status in the database (optional)
    logout(db, username); // Mark the user as logged out in the database
}
