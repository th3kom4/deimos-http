#include <iostream>
#include <memory>
#include <algorithm>
#include <csignal>

#include "HttpServer.hpp"
#include "StaticRouter.hpp"
#include "ServerState.hpp"

#include "LoggingMiddleware.hpp"
#include "SecurityMiddleware.hpp"
#include "RouterMiddleware.hpp"
#include "StaticFileMiddleware.hpp"
#include "FallbackMiddleware.hpp"

using namespace std;

void signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGTERM) {
		g_running.store(false, std::memory_order_relaxed);
	}
}

int main(int argc, char *argv[]) {
	std::signal(SIGPIPE, SIG_IGN);
	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);

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

	DynamicRouter api_router;

	api_router.get("/api/search", [](const HttpRequest& req) {
    std::string_view query = req.get_query_params("q");

	HttpResponse res;
	res.set_status(200, "OK")
		.set_body("You searched for: " + std::string(query));
	return res;
	});

	api_router.post("/api/echo", [](const HttpRequest& req) {
    std::string_view body = req.get_body();

	HttpResponse res;
	res.set_status(200, "OK")
	.set_body("Server received POST body: " + std::string(body));
	return res;
	});

	StaticRouter router("public");
	
	auto fallback_node = std::make_unique<FallbackMiddleware>();
	auto static_node = std::make_unique<StaticFileMiddleware>(&router);
	auto router_node = std::make_unique<RouterMiddleware>(&api_router);
	auto security_node = std::make_unique<SecurityMiddleware>();
	auto pipeline_head = std::make_unique<LoggingMiddleware>();

	static_node->set_next(std::move(fallback_node));
	router_node->set_next(std::move(static_node));
	security_node->set_next(std::move(router_node));
	pipeline_head->set_next(std::move(security_node));

	try {
		HttpServer server(port, worker_threads, pipeline_head.get());
		
		std::cout << "[INIT] Hardware threads detected: " << (hw_threads > 0 ? std::to_string(hw_threads) : "Unknown") << "\n";
		std::cout << "[INIT] Starting server on port " << port << " with " << worker_threads << " worker threads...\n";

		server.run();
	} catch (const std::exception& e) {
		std::cerr << "[FATAL ERROR] " << e.what() << "\n";
		return 1;
	}
	return 0;
}
