#include "FallbackMiddleware.hpp"

HttpResponse FallbackMiddleware::invoke(const HttpRequest&) {
	HttpResponse fallback;
	fallback.set_status(404, "Not Found")
			.add_header("Content-Type", "text/html")
			.set_body("<h1>404 - Page Not Found</h1>");
	
	return fallback;
}
