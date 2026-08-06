#include "DynamicRouter.hpp"

void DynamicRouter::get(const std::string& path, Handler handler) {
	routing_table["GET"][path] = std::move(handler);
}

void DynamicRouter::post(const std::string& path, Handler handler) {
	routing_table["POST"][path] = std::move(handler);
}

std::optional<HttpResponse> DynamicRouter::route_request(const HttpRequest& request) {
	std::string_view method = request.get_method();
	std::string_view uri = request.get_uri();

	auto method_it = routing_table.find(method);
	if (method_it != routing_table.end()) {
		auto route_it = method_it->second.find(std::string(uri));
		if (route_it != method_it->second.end()) {
			return route_it->second(request);
		}
	}

	return std::nullopt;
}
