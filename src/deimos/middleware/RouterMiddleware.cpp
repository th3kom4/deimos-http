#include "deimos/middleware/RouterMiddleware.hpp"

RouterMiddleware::RouterMiddleware(DynamicRouter* router_ptr)
	: router(router_ptr)
{
}

HttpResponse RouterMiddleware::invoke(const HttpRequest& request) {
	std::optional<HttpResponse> api_response = router->route_request(request);

	if (api_response.has_value()) {
		return api_response.value();
	}

	if (next) {
		return next->invoke(request);
	}

	HttpResponse fallback;
	fallback.set_status(404, "Not Found")
			.set_body("404 - Not Found");
	return fallback;
}
