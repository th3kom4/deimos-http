#include "HttpRequest.hpp"

#include <stdexcept>

std::string_view HttpRequest::get_header(const std::string_view key) const {
	auto it = headers.find(key);

	if (it != headers.end()) {
		return it->second;
	}

	return "";
}

HttpRequest::HttpRequest(std::string_view raw_data) {
	// Parse the request line (e.g., "GET /api HTTP/1.1\r\n")
	size_t end_of_line = raw_data.find("\r\n");
	if (end_of_line == std::string_view::npos) {
		throw std::invalid_argument("Malformed request: Missing request line");
	}
	
	std::string_view request_line = raw_data.substr(0, end_of_line);
	
	size_t space_pos = request_line.find(' ');
	if (space_pos == std::string_view::npos) {
		throw std::invalid_argument("Missing method");
	}
	method = request_line.substr(0, space_pos);

	request_line.remove_prefix(space_pos + 1);

	space_pos = request_line.find(' ');
	if (space_pos == std::string_view::npos) {
		throw std::invalid_argument("Missing URI");
	}
	uri = request_line.substr(0, space_pos);
	
	request_line.remove_prefix(space_pos + 1);

	version = request_line;

	raw_data.remove_prefix(end_of_line + 2);

	// Parse the headers
	while (!raw_data.empty()) {
		end_of_line = raw_data.find("\r\n");
		if (end_of_line == 0) {
			raw_data.remove_prefix(2);
			break;
		}

		std::string_view header_line = raw_data.substr(0, end_of_line);

		size_t colon_pos = header_line.find(':');
		if (colon_pos != std::string_view::npos) {
			std::string_view key = header_line.substr(0, colon_pos);
			header_line.remove_prefix(colon_pos + 1);

			while (!header_line.empty() && header_line.front() == ' ') {
				header_line.remove_prefix(1);
			}

			std::string_view value = header_line;

			headers[key] = value;
		}

		raw_data.remove_prefix(end_of_line + 2);
	}
	
	body = raw_data;
}
