#include "StaticRouter.hpp"

StaticRouter::StaticRouter(const std::string document_root)
	: base_directory(std::move(document_root)),
	  mime_directory({
          {".html", "text/html"},
          {".css",  "text/css"},
          {".js",   "application/javascript"},
          {".png",  "image/png"},
          {".jpg",  "image/jpeg"},
          {".jpeg", "image/jpeg"},
          {".gif",  "image/gif"},
          {".ico",  "image/x-icon"},
          {".json", "application/json"},
          {".txt",  "text/plain"}
      })	
{}

bool StaticRouter::is_path_safe(const std::string& uri) const {
	if (uri.find("..") != std::string::npos) {
		return false;
	}
	
	if (uri.find("/.") != std::string::npos) {
		return false;
	}

	return true;
}

std::string StaticRouter::resolve_path(const std::string& uri) const {
	if (!is_path_safe(uri)) {
		return "";
	}

	std::string normilized_uri = uri;
	if (uri == "/") {
		normilized_uri = "/index.html";
	}

	return base_directory + normilized_uri;
}

std::string StaticRouter::get_mime_type(const std::string& filepath) const {
	size_t dot_position = filepath.rfind('.');

	if (dot_position == std::string::npos || dot_position == filepath.length() - 1) {
		return "application/octet-stream";
	}

	std::string extension = filepath.substr(dot_position);

	auto it = mime_directory.find(extension);

	if (it != mime_directory.end()) {
		return it->second;
	}

	return "application/octet-stream";
}
