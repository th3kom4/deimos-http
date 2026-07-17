#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {
	struct sockaddr_in addr, cli_addr;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("Can't open port");
		exit(1);
	}

	int portno = atoi(argv[1]);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("Error on binding");
		exit(1);
	}
	listen(fd, 5);

	socklen_t cli_len = sizeof(cli_addr);
	int newsockfd = accept(fd, (struct sockaddr *) &cli_addr, &cli_len);

	string response = "HTTP/1.1 200 OK\n\nHello, World!";
	int n = write(newsockfd, response.c_str(), strlen(response.c_str()));
	if (n < 0) {
		perror("Error responding");
		exit(1);
	}
	cout << "Hello World!\n";
	return 0;
}
