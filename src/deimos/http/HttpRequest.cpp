#include "deimos/http/HttpRequest.hpp"

#include <stdexcept>

std::string_view HttpRequest::get_query_params(std::string_view key) const {
	auto it = query_params.find(key);
	if (it != query_params.end()) {
		return it->second;
	}
	return "";
}

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
	std::string_view full_uri = request_line.substr(0, space_pos);

	size_t question_mark_pos = full_uri.find('?');
	if (question_mark_pos != std::string_view::npos) {
		uri = full_uri.substr(0, question_mark_pos);
		std::string_view query_string = full_uri.substr(question_mark_pos + 1);
	
		while (!query_string.empty()) {
			size_t ampersand_pos = query_string.find('&');
			std::string_view pair = query_string.substr(0, ampersand_pos);

			size_t equals_pos = pair.find('=');
			if (equals_pos != std::string_view::npos) {
				std::string_view key = pair.substr(0, equals_pos);
				std::string_view value = pair.substr(equals_pos + 1);
				query_params[key] = value;
			} else {
				query_params[pair] = "";
			}

			if (ampersand_pos == std::string_view::npos) {
				break;
			}

			query_string.remove_prefix(ampersand_pos + 1);
		}
	} else {
		uri = full_uri;
	}
	
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
