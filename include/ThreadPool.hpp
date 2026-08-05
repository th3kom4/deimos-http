#pragma once

#include <vector>
#include <thread>
#include <functional>
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
	std::function<void(ClientConnection*)> close_connection_cb;
public:
	ThreadPool(size_t num_threads, TaskQueue& queue, IMiddleware* pipeline, int epoll_fd,
			   std::function<void(ClientConnection*)> close_cb);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	~ThreadPool();

	void shutdown();
};
