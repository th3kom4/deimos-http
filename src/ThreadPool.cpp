#include "ThreadPool.hpp"

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/epoll.h>

ThreadPool::ThreadPool(size_t num_threads, TaskQueue& q, IMiddleware* pipeline, int epoll_fd)
	: queue(q), pipeline_head(pipeline), epoll_fd(epoll_fd) {
	for (size_t i = 0; i < num_threads; ++i) {
		workers.emplace_back([this]() {
			this->worker_loop();
		});
	}
}

void ThreadPool::rearm_epoll(ClientConnection* client) {
	epoll_event event{};
	if (client->get_state() == ConnectionState::Reading) {
		event.events = EPOLLIN | EPOLLONESHOT;
	} else if (client->get_state() == ConnectionState::Writing) {
		event.events = EPOLLOUT | EPOLLONESHOT;
	}

	event.data.ptr = client;

	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->get_fd(), &event);
}

void ThreadPool::worker_loop() {
	while (true) {
		ClientConnection* client = queue.pop();
		if (client == nullptr) {
			break;
		}

		if (client->get_state() == ConnectionState::Reading) {
			client->read_request();
		}

		if (client->get_state() == ConnectionState::Processing) {
			try {
				HttpRequest request(client->get_read_buffer());
				
				HttpResponse response = pipeline_head->invoke(request);
				response.add_header("Connection", "close");

				client->set_write_buffer(response.serialize());
				client->clear_read_buffer();
				client->set_state(ConnectionState::Writing);
			} catch (const std::invalid_argument& e) {
				HttpResponse error_res;
				error_res.set_status(400, "Bad Request")
						 .add_header("Connection", "close")
						 .set_body("Malformed Request Data");
				
				client->set_write_buffer(error_res.serialize());
				client->clear_read_buffer();
				client->set_state(ConnectionState::Writing);
			} catch (const std::exception& e) {
				HttpResponse error_res;
				error_res.set_status(500, "Internal Server Error")
						 .add_header("Connection", "close")
						 .set_body("The server encountered an unexpected condition.");
				
				client->set_write_buffer(error_res.serialize());
				client->clear_read_buffer();
				client->set_state(ConnectionState::Writing);
			}
		}

		if (client->get_state() == ConnectionState::Writing) {
			client->send_response();
		}

		if (client->get_state() == ConnectionState::Finished) {
			delete client;
		} else {
			rearm_epoll(client);
		}
	}
}

void ThreadPool::shutdown() {
	queue.shutdown();

	for (std::thread& worker : workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
}

ThreadPool::~ThreadPool() {
	shutdown();
}
