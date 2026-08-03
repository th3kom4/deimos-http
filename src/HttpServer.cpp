#include "HttpServer.hpp"

#include <iostream>
#include "ServerState.hpp"

HttpServer::HttpServer(uint16_t port, size_t num_threads, IMiddleware* pipeline)
	: master_socket(port),
	  epoll_fd(epoll_create1(0)),
	  thread_pool(num_threads, task_queue, pipeline, epoll_fd) {
	if (epoll_fd == -1) {
		throw std::runtime_error("Failed to create epoll instance");
	}

	setup_epoll();	
}

void HttpServer::setup_epoll() {
	epoll_event event{};
	event.events = EPOLLIN;
	event.data.ptr = nullptr;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, master_socket.get_fd(), &event) == -1) {
		throw std::runtime_error("Failed to add master socket to epoll");
	}
 }

 void HttpServer::handle_new_connection() {
	ClientConnection* new_client = master_socket.accept_connection();

	if (new_client == nullptr) {
		return;
	}

	epoll_event event{};
	event.events = EPOLLIN | EPOLLONESHOT;
	event.data.ptr = new_client;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client->get_fd(), &event) == -1) {
		delete new_client;
	}
 }

 void HttpServer::run() {
	epoll_event events[MAX_EVENTS];
	std::cout << "[SERVER] Running. Press Ctrl+C to shut down.\n";

	while (g_running.load(std::memory_order_relaxed)) {
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 100);
		
		if (num_events < 0) {
			if (errno == EINTR) continue;
			break;
		}
		for (int i = 0; i < num_events; ++i) {
			if (events[i].data.ptr == nullptr) {
				handle_new_connection();
			} else {
				ClientConnection* client = static_cast<ClientConnection*>(events[i].data.ptr);
				task_queue.push(client);
			}
		}
	}

	stop();
 }

 void HttpServer::stop() {
	std::cout << "\n[SERVER] Initiating graceful shutdown...\n";

	task_queue.shutdown();
	thread_pool.shutdown();

	if (epoll_fd >= 0) {
		close(epoll_fd);
		epoll_fd = -1;
	}

	std::cout << "[SERVER] Thread pool joined and epoll closed. Shutdown complete.\n";
 }

 HttpServer::~HttpServer() {
	if (epoll_fd >= 0) {
		close(epoll_fd);
	}
 }
