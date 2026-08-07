#pragma once

#include <memory>
#include "deimos/http/HttpRequest.hpp"
#include "deimos/http/HttpResponse.hpp"

class IMiddleware {
protected:
	std::unique_ptr<IMiddleware> next;

public:
	virtual ~IMiddleware() = default;
	virtual void set_next(std::unique_ptr<IMiddleware> next_node) {
		next = std::move(next_node);
	}
	virtual HttpResponse invoke(const HttpRequest& request) = 0;
};
