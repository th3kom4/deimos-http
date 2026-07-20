#include "ClientConnection.hpp"

#include <sys/socket.h>
#include <stdexcept>

ClientConnection::ClientConnection(int fd) : client_fd(fd) {}

ClientConnection::~ClientConnection() {
	if (client_fd >= 0) {
		close(client_fd);
	}
}

std::string ClientConnection::read_request() {
	char buffer[4096] = {0};

	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read < 0) {
		throw std::runtime_error("Failed to read from socket.");
	} else if (bytes_read == 0) {
		return "";
	}

	return std::string(buffer, bytes_read);
}

void ClientConnection::send_response(const std::string& response) {
	size_t total_sent = 0;
	size_t length = response.length();
	const char* data = response.c_str();

	while (total_sent < length) {
		ssize_t bytes_sent = send(client_fd, data + total_sent, length - total_sent, 0);

		if (bytes_sent < 0) {
			throw std::runtime_error("Failed to send response to socket.");
		}

		total_sent += bytes_sent;
	}
}
