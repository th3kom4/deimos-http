#pragma once

#include <string>
#include <unistd.h>

enum class ConnectionState {
	Reading,
	Processing,
	Writing,
	Finished
};

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

	void read_request();
	void send_response();

	int get_fd() const { return client_fd; }
	ConnectionState get_state() const { return state; }
	void set_state(ConnectionState new_state) { state = new_state; }
	const std::string& get_read_buffer() const { return read_buffer; }
	void clear_read_buffer() { read_buffer.clear(); }
	void set_write_buffer(const std::string& data) { write_buffer = data; }

private:
	int client_fd;
	ConnectionState state;
	std::string read_buffer;
	std::string write_buffer;
 };
