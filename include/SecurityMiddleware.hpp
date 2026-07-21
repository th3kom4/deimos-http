#pragma once

#include "IMiddleware.hpp"

class SecurityMiddleware : public IMiddleware {
public:
	HttpResponse invoke(const HttpRequest& request) override;
};
