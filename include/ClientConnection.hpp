#pragma once

#include <string>
#include <chrono>
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
	void reset();
	bool is_keep_alive() const { return keep_alive; }
	void set_keep_alive(bool ka) { keep_alive = ka; }
	void update_heartbeat() { last_active = std::chrono::steady_clock::now(); }
	bool is_timed_out(int timeout_seconds) const;

private:
	int client_fd;
	ConnectionState state;
	std::string read_buffer;
	std::string write_buffer;
	bool keep_alive;
	std::chrono::steady_clock::time_point last_active{std::chrono::steady_clock::now()};
 };
