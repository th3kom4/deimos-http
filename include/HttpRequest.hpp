#pragma once

#include <unordered_map>
#include <string>

class HttpRequest {
public:
	HttpRequest(std::string raw_data);

	const std::string& get_method() const;
	const std::string& get_uri() const;
	const std::string& get_version() const;
	const std::string& get_body() const;

	std::string get_header(const std::string& key) const;

private:
	std::string method;
	std::string uri;
	std::string version;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
};
