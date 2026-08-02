#pragma once

#include <vector>
#include <thread>
#include "TaskQueue.hpp"
#include "ClientConnection.hpp"
#include "IMiddleware.hpp"

class ThreadPool {
private:
	std::vector<std::thread> workers;
	TaskQueue& queue;
	IMiddleware* pipeline_head;
	int epoll_fd;

	void worker_loop();
	void rearm_epoll(ClientConnection* client);

public:
	ThreadPool(size_t num_threads, TaskQueue& queue, IMiddleware* pipeline, int epoll_fd);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	~ThreadPool();

	void shutdown();
};
