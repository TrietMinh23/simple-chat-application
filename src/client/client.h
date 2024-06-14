#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;
const std::string DELIM("$$$");

// Client utility functions
int Socket();
void Connect(int sockfd, const std::string &host, int port);
void clientChat(int sockfd);
std::vector<std::string> split(const std::string &str, const std::string &delim);

#endif // CLIENT_H_
