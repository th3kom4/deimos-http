#include <iostream>
#include "TcpSocket.hpp"
#include "ClientConnection.hpp"

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
			if (!data.empty()) {
				std::cout << "Received request:\n" << data << "\n";
			}

			std::string response = "HTTP/1.1 200 OK\r\n\r\nHello, World!"; 
			client.send_response(response);
		}
	} catch (const std::exception& e) {
		std::cerr << "Server Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
