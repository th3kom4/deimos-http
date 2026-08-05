#pragma once

#include <sys/epoll.h>
#include <unordered_set>
#include <mutex>
#include "TaskQueue.hpp"
#include "ThreadPool.hpp"
#include "TcpSocket.hpp"

constexpr int MAX_EVENTS = 1024;

class HttpServer {
private:
	TcpSocket master_socket;
	int epoll_fd;

	TaskQueue task_queue;
	ThreadPool thread_pool;
	std::unordered_set<ClientConnection*> active_connections;
	std::mutex registry_mutex;

	void setup_epoll();
	void handle_new_connection();
	void remove_connection(ClientConnection* client);

public:
	HttpServer(uint16_t port, size_t num_threads, IMiddleware* pipeline);
	~HttpServer();

	HttpServer(const HttpServer&) = delete;
	HttpServer& operator=(const HttpServer&) = delete;

	void run();
	void stop();
};
