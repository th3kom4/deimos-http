#include <iostream>
#include <memory>
#include <algorithm>

#include "HttpServer.hpp"
#include "StaticRouter.hpp"

#include "LoggingMiddleware.hpp"
#include "SecurityMiddleware.hpp"
#include "StaticFileMiddleware.hpp"
#include "FallbackMiddleware.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 2 || argc > 3) {
		std::cerr << "Usage: " << argv[0] << " <port> [worker_threads]\n";
		return 1;
	}

	int port = atoi(argv[1]);

	unsigned int hw_threads = std::thread::hardware_concurrency();
	unsigned int default_threads = (hw_threads > 0) ? std::max(1u, hw_threads - 1) : 4;

	unsigned int worker_threads = default_threads;
	if (argc > 2) {
		worker_threads = std::max(1, std::atoi(argv[2]));
	}


	StaticRouter router("public");
	
	auto fallback_node = std::make_unique<FallbackMiddleware>();
	auto static_node = std::make_unique<StaticFileMiddleware>(&router);
	auto security_node = std::make_unique<SecurityMiddleware>();
	auto pipeline_head = std::make_unique<LoggingMiddleware>();

	static_node->set_next(std::move(fallback_node));
	security_node->set_next(std::move(static_node));
	pipeline_head->set_next(std::move(security_node));

	HttpServer server(port, worker_threads, pipeline_head.get());
	
	std::cout << "[INIT] Hardware threads detected: " << (hw_threads > 0 ? std::to_string(hw_threads) : "Unknown") << "\n";
	std::cout << "[INIT] Starting server on port " << port << " with " << worker_threads << " worker threads...\n";

	server.run();

	return 0;
}
