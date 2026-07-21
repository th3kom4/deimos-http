#include <iostream>
#include "TcpSocket.hpp"
#include "ClientConnection.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "FileSystemHandler.hpp"
#include "StaticRouter.hpp"

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

		while (true) {
			ClientConnection client = server.accept_connection();
			std::string data = client.read_request();

			if (data.empty()) continue;

			try {
				HttpRequest request(data);

				std::cout << "\n========== NEW REQUEST ==========\n";
                std::cout << "Method: " << request.get_method() << "\n";
                std::cout << "URI:    " << request.get_uri() << "\n";
                std::cout << "Host:   " << request.get_header("Host") << "\n";
                std::cout << "Agent:  " << request.get_header("User-Agent") << "\n";
                std::cout << "=================================\n";

				std::string uri = request.get_uri();

				std::string filepath = router.resolve_path(uri);
				if (filepath.empty()) {
					continue;
				}

                auto file_data = FileSystemHandler::read_file(filepath);

				std::string mime_type = router.get_mime_type(filepath);
			
				HttpResponse response;
				response.add_header("Connection", "close");

				if (file_data.has_value()) {
					response.set_status(200, "OK")
							.add_header("Content-Type", mime_type)
							.set_body(std::move(file_data.value()));
				} else {
					response.set_status(404, "Not Found")
							.add_header("Content-Type", mime_type)
							.set_body("<h1>404 - File Not Found</h1>");
				}

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
