#include "TaskQueue.hpp"

void TaskQueue::push(ClientConnection* client) {
	std::lock_guard<std::mutex> lock(mtx);
	tasks.push(client);
	cv.notify_one();
}

ClientConnection* TaskQueue::pop() {
	std::unique_lock<std::mutex> lock(mtx);

	cv.wait(lock, [this]() {
		return !tasks.empty() || is_shutting_down;
	});

	if (is_shutting_down && tasks.empty()) {
		return nullptr;
	}

	ClientConnection* client = tasks.front();
	tasks.pop();

	return client;
}

void TaskQueue::shutdown() {
	std::lock_guard<std::mutex> lock(mtx);
	is_shutting_down = true;
	cv.notify_all();
}
