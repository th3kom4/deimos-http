#include "StaticFileMiddleware.hpp"

#include "FileSystemHandler.hpp"
#include <string_view>

StaticFileMiddleware::StaticFileMiddleware(StaticRouter* router_ptr) 
	: router(router_ptr)
{

}

HttpResponse StaticFileMiddleware::invoke(const HttpRequest& request) {
	std::string_view uri = request.get_uri();	
	std::string filepath = router->resolve_path(uri);

	auto file_data = FileSystemHandler::read_file(filepath);

	if (!file_data.has_value()) {
		if (next) {
			return next->invoke(request);
		}

		HttpResponse fallback;
		fallback.set_status(500, "Internal Server Error");
		return fallback;
	}

	HttpResponse response;
	response.set_status(200, "OK")
			.add_header("Content-Type", router->get_mime_type(filepath))
			.set_body(std::move(file_data.value()));

	return response;
}
