#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

class ClientConnection;

class TaskQueue {
private:
	std::queue<ClientConnection*> tasks;
	std::mutex mtx;
	std::condition_variable cv;
	bool is_shutting_down = false;

public:
	TaskQueue() = default;
	~TaskQueue() = default;

	void push(ClientConnection* client);
	ClientConnection* pop();
	void shutdown();
};
