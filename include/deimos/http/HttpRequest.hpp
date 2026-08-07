#pragma once

#include <unordered_map>
#include <string_view>
#include <cctype>
#include <cstdint>

// FNV-1a Algorithm
struct CaseInsensitiveHash {
	std::size_t operator()(std::string_view key) const {
		// FNV-1a offset basis (standard 64-bit prime initialization)
		std::size_t hash = 14695981039346656037ull;

		for (char c : key) {
			unsigned char lower_c = std::tolower(static_cast<unsigned char>(c));

			hash ^= lower_c;
			hash *= 1099511628211ull; // FNV prime
		}
		return hash;
	}
};

struct CaseInsensitiveEqual {
	bool operator()(std::string_view lhs, std::string_view rhs) const {
		if (lhs.size() != rhs.size()) {
			return false;
		}

		for (std::size_t i = 0; i < lhs.size(); ++i) {
			if (std::tolower(static_cast<unsigned char>(lhs[i])) !=
				std::tolower(static_cast<unsigned char>(rhs[i]))) {
				return false;
			}
		}
		return true;
	}
};

class HttpRequest {
public:
	HttpRequest(std::string_view raw_data);

	std::string_view get_method() const { return method; }
	std::string_view get_uri() const { return uri; }
	std::string_view get_version() const { return version; }
	std::string_view get_body() const { return body; }
	std::string_view get_query_params(std::string_view key) const;

	std::string_view get_header(const std::string_view key) const;

private:
	std::string_view method;
	std::string_view uri;
	std::unordered_map<std::string_view, std::string_view> query_params;
	std::string_view version;
	std::unordered_map<std::string_view,
					   std::string_view,
					   CaseInsensitiveHash,
					   CaseInsensitiveEqual> headers;
	std::string_view body;
};
