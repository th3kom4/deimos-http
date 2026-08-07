#pragma once

#include "deimos/http/HttpRequest.hpp"
#include "deimos/http/HttpResponse.hpp"
#include <unordered_map>
#include <functional>
#include <optional>

using Handler = std::function<HttpResponse(const HttpRequest&)>;

class DynamicRouter {
public:
	void get(const std::string& path, Handler handler);
	void post(const std::string& path, Handler handler);
	std::optional<HttpResponse> route_request(const HttpRequest& request);

private:
	// Outer map: HTTP Method (GET, POST)
	// Inner map: URI Path ("/api/status") mapped to the executable Handler
	std::unordered_map<std::string_view, std::unordered_map<std::string, Handler>> routing_table;
};
