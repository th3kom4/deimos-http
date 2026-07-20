#include "HttpRequest.hpp"

#include <stdexcept>

const std::string& HttpRequest::get_method() const {
	return method;
}

const std::string& HttpRequest::get_uri() const {
	return uri;
}

const std::string& HttpRequest::get_version() const {
	return version;
}

const std::string& HttpRequest::get_body() const {
	return body;
}

std::string HttpRequest::get_header(const std::string& key) const {
	auto it = headers.find(key);

	if (it != headers.end()) {
		return it->second;
	}

	return "";
}

HttpRequest::HttpRequest(std::string raw_data) {
	size_t current_pos = 0;
	size_t space_pos = raw_data.find(" ", current_pos);
	if (space_pos == std::string::npos) {
		throw std::invalid_argument("Malformed request: Missing method space");
	}
	method = raw_data.substr(current_pos, space_pos - current_pos);
	current_pos = space_pos + 1;

	space_pos = raw_data.find(" ", current_pos);
	if (space_pos == std::string::npos) {
		throw std::invalid_argument("Malformed request: Missing URI space");
	}
	uri = raw_data.substr(current_pos, space_pos - current_pos);
	current_pos = space_pos + 1;

	space_pos = raw_data.find("\r\n", current_pos);
	if (space_pos == std::string::npos) {
		throw std::invalid_argument("Malformed request: Missing version line break");
	}
	version = raw_data.substr(current_pos, space_pos - current_pos);
	current_pos = space_pos + 2;

	while (current_pos < raw_data.length() && raw_data.substr(current_pos, 2) != "\r\n") {
		size_t end_of_line_pos = raw_data.find("\r\n", current_pos);
		if (end_of_line_pos == std::string::npos) break;

		size_t colon_pos = raw_data.find(": ", current_pos);
		if (colon_pos == std::string::npos || colon_pos > end_of_line_pos) {
			current_pos = end_of_line_pos + 2;
			continue;
		}

		std::string key = raw_data.substr(current_pos, colon_pos - current_pos);
		std::string value = raw_data.substr(colon_pos + 2,  end_of_line_pos - (colon_pos + 2));

		headers[key] = value;

		current_pos = end_of_line_pos + 2;
	}

	current_pos += 2;

	if (current_pos < raw_data.length()) {
		body = raw_data.substr(current_pos);
	} else {
		body = "";
	}
}
