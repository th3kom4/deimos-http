#include "deimos/http/HttpResponse.hpp"

#include <string>
#include <stdexcept>

int HttpResponse::get_status_code() const {
	return status_code;
}

const std::vector<char>& HttpResponse::get_body() const {
	return body;
}

HttpResponse::HttpResponse() {
	version = "HTTP/1.1";
	status_code = 200;
	status_message = "OK";
}

HttpResponse& HttpResponse::set_status(int code, const std::string& message) {
	status_code = code;
	status_message = message;
	return *this;
}

HttpResponse& HttpResponse::add_header(const std::string& key, const std::string& value) {
	if (key.find('\r') != std::string::npos || key.find('\n') != std::string::npos ||
		value.find('\r') != std::string::npos || value.find('\n') != std::string::npos) {
		throw std::invalid_argument("CRLF Injection detected in HTTP header");
	}

	headers[key] = value;
	return *this;
}

HttpResponse& HttpResponse::set_body(std::vector<char>&& raw_data) {
	body = std::move(raw_data);
	return *this;
}

HttpResponse& HttpResponse::set_body(const std::string& text_data) {
	body.assign(text_data.begin(), text_data.end());
	return *this;
}

std::string HttpResponse::serialize() const {
	size_t total_size = 0;

	std::string body_size_str;
	if (body.size() > 0) {
		body_size_str = std::to_string(body.size());
		// "Content-Length" (14) + ": " (2) + "\r\n" (2) = 18 bytes
		total_size += 18 + body_size_str.length();
	}

	// Format: "<version> <code> <message>\r\n"
	total_size += version.length() + 3 + status_message.length() + 4;

	// Format: "<key>: <value>\r\n"
	for (const auto& pair : headers) {
		total_size += pair.first.length() + pair.second.length() + 4;
	}

	// Format: "\r\n"
	total_size += 2;

	total_size += body.size();

	std::string output;
	output.reserve(total_size);

	output.append(version);
	output.append(" ");
	output.append(std::to_string(status_code));
	output.append(" ");
	output.append(status_message);
	output.append("\r\n");
	
	if (body.size() > 0) {
		output.append("Content-Length: ");
		output.append(body_size_str);
		output.append("\r\n");
	}

	for (const auto& pair : headers) {
		output.append(pair.first);
		output.append(": ");
		output.append(pair.second);
		output.append("\r\n");
	}
	
	output.append("\r\n");

	output.append(body.begin(), body.end());

	return output;
}
