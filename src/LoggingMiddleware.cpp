#include "LoggingMiddleware.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

HttpResponse LoggingMiddleware::invoke(const HttpRequest& request) {
	HttpResponse response;
	if (next) {
		response = next->invoke(request);
	} else {
		response.set_status(500, "Internal Server Error");
	}

	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm local_tm;
	localtime_r(&now_c, &local_tm);

	size_t body_size = response.get_body().size();

	std::ostringstream log_buffer;
	log_buffer << "- "								// Placeholder for Client IP
			   << "- " 								// Placeholder for Identity
			   << "- "								// Placeholder for Auth User
			   << "[" << std::put_time(&local_tm, "%d/%b/%Y:%H:%M:%S %z") << "] "
			   << "\"" << request.get_method() << " " << request.get_uri() << " HTTP/1.1\" "
			   << response.get_status_code() << " "
			   << (body_size == 0 ? "-" : std::to_string(body_size))
			   << "\n";
	
	std::cout << log_buffer.str();

	return response;
}
