#pragma once

#include "deimos/middleware/IMiddleware.hpp"

class LoggingMiddleware : public IMiddleware {
public:
	HttpResponse invoke(const HttpRequest& request) override;
};
