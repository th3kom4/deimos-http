#include <iostream>
#include "TcpSocket.hpp"
#include "ClientConnection.hpp"
#include "HttpRequest.hpp"

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

                std::string html = "<h1>Parser Success!</h1><p>You requested: " + request.get_uri() + "</p>";

                std::string response = "HTTP/1.1 200 OK\r\n"
                                       "Content-Type: text/html\r\n"
                                       "Content-Length: " + std::to_string(html.length()) + "\r\n"
                                       "\r\n" + html;
			
				client.send_response(response);
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
