#include "./server.h"

int main(int argc, char *argv[])
{
  const int port = (argc > 1) ? std::atoi(argv[1]) : 8000;
  const int backlog = (argc > 2) ? std::atoi(argv[2]) : 5;

  const int sockfd = Socket();
  Bind(sockfd, port);
  Listen(sockfd, backlog);
  serverChat(sockfd);
  close(sockfd);
  return errno;
}
