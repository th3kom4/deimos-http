#pragma once

#include <string>
#include <unistd.h>

class ClientConnection {
public:
	ClientConnection(int fd);
	~ClientConnection();	

	ClientConnection(const ClientConnection& other) = delete;
	ClientConnection& operator=(const ClientConnection& other) = delete;

	ClientConnection(ClientConnection&& other) noexcept {
		this->client_fd = other.client_fd;
		other.client_fd = -1;
	}

	ClientConnection& operator=(ClientConnection&& other) noexcept {
		if (this != &other) {
			if (this->client_fd >= 0) {
				close(this->client_fd);
			}

			this->client_fd = other.client_fd;
			other.client_fd = -1;
		}

		return *this;
	}

	std::string read_request();
	void send_response(const std::string& response);

private:
	int client_fd;
 };
