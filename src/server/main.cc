#include "./server.h"
#include <signal.h>

void shutdownHandler(int sig)
{
	// Handle graceful shutdown, such as waiting for threads to finish
	cout << "Shutdown signal received: " << sig << endl;
	exit(sig);
}

int main(int argc, char *argv[])
{
	const int port = (argc > 1) ? std::atoi(argv[1]) : 8080;
	const int backlog = (argc > 2) ? std::atoi(argv[2]) : 5;

	signal(SIGINT, shutdownHandler); // Setup graceful shutdown on SIGINT

	const int sockfd = Socket();
	Bind(sockfd, port);
	Listen(sockfd, backlog);
	serverChat(sockfd);
	close(sockfd);
	return 0;
}
