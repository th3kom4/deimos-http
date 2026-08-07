#include "deimos/concurrency/ThreadPool.hpp"

#include "deimos/http/HttpRequest.hpp"
#include "deimos/http/HttpResponse.hpp"
#include <sys/epoll.h>

ThreadPool::ThreadPool(size_t num_threads, TaskQueue& q, IMiddleware* pipeline, int epoll_fd,
					   std::function<void(ClientConnection*)> close_cb)
	: queue(q), pipeline_head(pipeline), epoll_fd(epoll_fd), close_connection_cb(close_cb) {
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
				
				bool is_keep_alive = request.get_header("Connection") != "close";
				client->set_keep_alive(is_keep_alive);

				HttpResponse response = pipeline_head->invoke(request);
				if (!is_keep_alive) {
					response.add_header("Connection", "close");
				}

				client->set_write_buffer(response.serialize());
				client->clear_read_buffer();
				client->set_state(ConnectionState::Writing);
			} catch (const std::invalid_argument& e) {
				client->set_keep_alive(false);

				HttpResponse error_res;
				error_res.set_status(400, "Bad Request")
						 .add_header("Connection", "close")
						 .set_body("Malformed Request Data");
				
				client->set_write_buffer(error_res.serialize());
				client->clear_read_buffer();
				client->set_state(ConnectionState::Writing);
			} catch (const std::exception& e) {
				client->set_keep_alive(false);

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
			if (client->is_keep_alive()) {
				client->reset();
				rearm_epoll(client);
			} else {
				close_connection_cb(client);
			}
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
