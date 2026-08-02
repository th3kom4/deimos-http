#include "ClientConnection.hpp"

#include <sys/socket.h>
#include <stdexcept>

ClientConnection::ClientConnection(int fd)
	: client_fd(fd), state(ConnectionState::Reading) {}

ClientConnection::~ClientConnection() {
	if (client_fd >= 0) {
		close(client_fd);
	}
}

void ClientConnection::read_request() {
	char buffer[4096] = {0};

	while (true) {
		ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

		if (bytes_read > 0) {
			read_buffer.append(buffer, bytes_read);
		} else if (bytes_read == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				state = ConnectionState::Finished;
				return;
			}
		} else if (bytes_read == 0) {
			state = ConnectionState::Finished;
			return;
		}
	}

	if (read_buffer.find("\r\n\r\n") != std::string::npos) {
		state = ConnectionState::Processing;
	}
}

void ClientConnection::send_response() {
	while (!write_buffer.empty()) {
		ssize_t bytes_sent = send(client_fd, write_buffer.c_str(), write_buffer.length(), 0);

		if (bytes_sent >  0) {
			write_buffer.erase(0, bytes_sent);
		} else if (bytes_sent == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				state = ConnectionState::Finished;
				return;
			}
		}
	}

	if (write_buffer.empty()) {
		state = ConnectionState::Finished;
	}
}
