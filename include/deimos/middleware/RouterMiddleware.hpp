#pragma once

#include "deimos/middleware/IMiddleware.hpp"
#include "deimos/routing/DynamicRouter.hpp"

class RouterMiddleware : public IMiddleware {
private:
	DynamicRouter* router;

public:
	RouterMiddleware(DynamicRouter* router_ptr);
	HttpResponse invoke(const HttpRequest& request);
};
