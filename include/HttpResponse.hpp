#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class HttpResponse {
public:
	HttpResponse();

	HttpResponse& set_status(int code, const std::string& message);
	HttpResponse& add_header(const std::string& key, const std::string& value);

	HttpResponse& set_body(std::vector<char>&& raw_data);
	HttpResponse& set_body(const std::string& text_data);

	std::string serialize() const;

	int get_status_code() const;
	const std::vector<char>& get_body() const;

private:
	std::string version;
	int status_code;
	std::string status_message;
	std::unordered_map<std::string, std::string> headers;
	std::vector<char> body;
};
