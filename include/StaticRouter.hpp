#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

class StaticRouter {
private:
	std::string base_directory;
	std::unordered_map<std::string, std::string> mime_directory;

	bool is_path_safe(const std::string_view uri) const;

public:
	StaticRouter(const std::string document_root);

	std::string resolve_path(const std::string_view uri) const;
	std::string get_mime_type(const std::string& filepath) const;

};
