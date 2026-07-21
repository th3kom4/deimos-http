#pragma once

#include "IMiddleware.hpp"

class FallbackMiddleware : public IMiddleware {
public:
	HttpResponse invoke(const HttpRequest& request) override;
};
