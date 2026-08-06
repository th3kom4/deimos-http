#pragma once

#include "IMiddleware.hpp"
#include "DynamicRouter.hpp"

class RouterMiddleware : public IMiddleware {
private:
	DynamicRouter* router;

public:
	RouterMiddleware(DynamicRouter* router_ptr);
	HttpResponse invoke(const HttpRequest& request);
};
