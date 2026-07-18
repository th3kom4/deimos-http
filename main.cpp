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
	char buf[4096];
	int n;
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
	while(1) {
	socklen_t cli_len = sizeof(cli_addr);

	int newsockfd = accept(fd, (struct sockaddr *) &cli_addr, &cli_len);
		
	n = read(newsockfd, buf, 4095);
	if (n < 0) {
		perror("Error reading request");
		exit(1);
	}
	printf("%s\n", buf);
		
	char response[] = "HTTP/1.1 200 OK\n\nHello, World!";

	n = write(newsockfd, response, strlen(response));
	if (n < 0) {
		perror("Error responding");
		exit(1);
	}
	close(newsockfd);
	}
	return 0;
}
