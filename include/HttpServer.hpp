#pragma once

#include <sys/epoll.h>
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

	void setup_epoll();
	void handle_new_connection();

public:
	HttpServer(uint16_t port, size_t num_threads, IMiddleware* pipeline);
	~HttpServer();

	HttpServer(const HttpServer&) = delete;
	HttpServer& operator=(const HttpServer&) = delete;

	void run();
	void stop();
};
