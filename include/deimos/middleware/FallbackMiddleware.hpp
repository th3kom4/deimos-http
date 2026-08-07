#pragma once

#include "deimos/middleware/IMiddleware.hpp"

class FallbackMiddleware : public IMiddleware {
public:
	HttpResponse invoke(const HttpRequest& request) override;
};
