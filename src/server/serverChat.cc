#include "./server.h"
#include <thread>
#include <map>
#include <mutex>

std::map<int, std::thread> clientThreads;
std::mutex clientsMutex;

int serverChat(int sockfd)
{
	Database<User> db("./data/users/", "name");
	fd_set master;
	FD_ZERO(&master);
	FD_SET(sockfd, &master);

	while (true)
	{
		fd_set readfds = master;
		int maxfd = sockfd; // Update for the highest fd in the set if necessary

		if (select(maxfd + 1, &readfds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(sockfd, &readfds))
		{
			int newClient = Accept(sockfd);
			if (newClient < 0)
			{
				perror("accept");
				continue;
			}

			std::lock_guard<std::mutex> lock(clientsMutex);
			clientThreads[newClient] = std::thread([newClient, &db]()
												   {
                handleClient(newClient, db);
                close(newClient); });
			clientThreads[newClient].detach();
		}
	}

	// Cleanup: join all threads on shutdown
	std::lock_guard<std::mutex> lock(clientsMutex);
	for (auto &entry : clientThreads)
	{
		if (entry.second.joinable())
			entry.second.join();
	}

	return 0;
}

void handleClient(int clientSocket, Database<User> &db)
{
	char buffer[BUFFER_SIZE];
	FdToName clients;
	ChatroomToFdList chatRooms;

	while (true)
	{
		std::fill_n(buffer, BUFFER_SIZE, '\0');
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0)
		{
			handleLostConnection(clientSocket, chatRooms, clients, db);
			break;
		}

		string msg(buffer);
		auto type = getMessageType(msg);
		if (type == cmd::NOT_CMD)
		{
			handleMsg(clientSocket, chatRooms, clients, msg);
			continue;
		}

		switch (type)
		{
		case cmd::JOIN:
		{
			auto chatRoomName = split(msg, DELIM, 2)[1];
			cout << color::green << "[CLIENT:JOIN] " << color::reset
				 << "<" << clients[clientSocket] << "> requested to join #" << chatRoomName << endl;
			joinChatRoom(chatRoomName, clientSocket, clients, chatRooms);
			continue;
		}

		case cmd::LEAVE:
		{
			auto chatRoomName = split(msg, DELIM, 2)[1];
			cout << color::green << "[CLIENT:LEAVE] " << color::reset
				 << "<" << clients[clientSocket] << "> left #" << chatRoomName << endl;
			leaveChatRoom(chatRoomName, clientSocket, clients, chatRooms);
			continue;
		}

		case cmd::LIST_CHATROOMS:
		{
			auto response = getChatroomsList(chatRooms);
			send(clientSocket, response.c_str(), response.size(), 0);
			continue;
		}

		case cmd::LIST_PEOPLE:
		{
			auto chatRoomName = split(msg, DELIM, 2)[1];
			auto list = getPeopleList(chatRoomName, clients, chatRooms);
			auto response = "INFO" + DELIM + "PEOPLE#" + chatRoomName + DELIM + list;
			send(clientSocket, response.c_str(), response.size(), 0);
			continue;
		}

		case cmd::SELF:
		{
			std::string response = DELIM + "SELF_RESPONSE";
			send(clientSocket, response.c_str(), response.size(), 0);
			cout << color::cyan << "[SERVER:SELF] " << color::reset
				 << "Responded to client's SELF request" << endl;
			continue;
		}

		case cmd::INVALID:
		default:
			continue;
		}
	}
}
