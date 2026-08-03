#include "TcpSocket.hpp"
#include "ClientConnection.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <unistd.h>

void TcpSocket::make_non_blocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		throw std::runtime_error(std::string("fcntl F_GETFL failed: ") + strerror(errno));
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw std::runtime_error(std::string("fcntl F_SETFL failed: ") + strerror(errno));
	}
}

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

	make_non_blocking(sock_fd);

	if (listen(sock_fd, SOMAXCONN) < 0) {
		throw std::runtime_error(std::string("Listen failed: ") + strerror(errno));
	}
}

TcpSocket::~TcpSocket() {
	if (sock_fd >= 0) {
		close(sock_fd);
	}
}

ClientConnection* TcpSocket::accept_connection() {
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);

	int new_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &cli_len);
	if (new_fd < 0) {
		// No clients waiting to connect 
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return nullptr;
		}
		// Client aborted connection or OS interrupted syscall
		if (errno == EINTR || errno == ECONNABORTED) {
			return nullptr;
		}
		// Out of file descriptors
		if (errno == EMFILE || errno == ENFILE) {
			return nullptr;
		}
		throw std::runtime_error(std::string("Accept failed: ") + strerror(errno));
	}
	try {
		make_non_blocking(new_fd);
	} catch (const std::exception& e) {
		close(new_fd);
		return nullptr;
	}
	return new ClientConnection(new_fd);
}
