#include <iostream>
#include <memory>

#include "TcpSocket.hpp"
#include "ClientConnection.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "StaticRouter.hpp"

#include "LoggingMiddleware.hpp"
#include "SecurityMiddleware.hpp"
#include "StaticFileMiddleware.hpp"
#include "FallbackMiddleware.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port>\n";
		return 1;
	}

	try {
		int portno = atoi(argv[1]);
		TcpSocket server(portno);	
		std::cout << "Server listening on port " << portno << "...\n";
		
		StaticRouter router("public");

		auto fallback_node = std::make_unique<FallbackMiddleware>();
		auto static_node = std::make_unique<StaticFileMiddleware>(&router);
		auto security_node = std::make_unique<SecurityMiddleware>();
		auto pipeline_head = std::make_unique<LoggingMiddleware>();

		static_node->set_next(std::move(fallback_node));
		security_node->set_next(std::move(static_node));
		pipeline_head->set_next(std::move(security_node));

		while (true) {
			ClientConnection client = server.accept_connection();
			std::string data = client.read_request();

			if (data.empty()) continue;

			try {
				HttpRequest request(data);

				HttpResponse response = pipeline_head->invoke(request);
				
				response.add_header("Connection", "close");

				client.send_response(response.serialize());
			} catch (const std::invalid_argument& e) {
				std::cerr << "[WARNING] Dropped malformed request: " << e.what() << "\n";
			}
		}
	} catch (const std::exception& e) {
		std::cerr << "Server Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
