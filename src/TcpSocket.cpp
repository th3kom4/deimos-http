#include "TcpSocket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "ClientConnection.hpp"

TcpSocket::TcpSocket(int portno) {
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		throw std::runtime_error(std::string("Socket creation failed: ") + strerror(errno));
	}

	int opt = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		throw std::runtime_error(std::string("Bind failed: ") + strerror(errno));
	}

	if (listen(sock_fd, 5) < 0) {
		throw std::runtime_error(std::string("Listen failed: ") + strerror(errno));
	}
}

TcpSocket::~TcpSocket() {
	if (sock_fd >= 0) {
		close(sock_fd);
	}
}

ClientConnection TcpSocket::accept_connection() {
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);

	int new_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &cli_len);
	if (new_fd < 0) {
		throw std::runtime_error(std::string("Accept failed: ") + strerror(errno));
	}

	return ClientConnection(new_fd);
}
