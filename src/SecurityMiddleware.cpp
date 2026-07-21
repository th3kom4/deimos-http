#include "SecurityMiddleware.hpp"

#include <string>

HttpResponse SecurityMiddleware::invoke(const HttpRequest& request) {
	std::string uri = request.get_uri();

	if ((uri.find("..") != std::string::npos) || 
		(uri.find("/.") != std::string::npos)) {
		HttpResponse response;
		response.set_status(403, "Forbidden")
				.set_body("Access Denied");
		return response;
	}

	if (next) {
		return next->invoke(request);
	}

	HttpResponse fallback;
	fallback.set_status(500, "Internal Server Error");
	return fallback;
}
