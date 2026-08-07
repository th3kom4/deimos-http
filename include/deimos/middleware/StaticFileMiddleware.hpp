#pragma once

#include "deimos/middleware/IMiddleware.hpp"
#include "deimos/routing/StaticRouter.hpp"

class StaticFileMiddleware : public IMiddleware {
private:
	StaticRouter* router;

public:
	explicit StaticFileMiddleware(StaticRouter* router_ptr);

	HttpResponse invoke(const HttpRequest& request) override;
};
