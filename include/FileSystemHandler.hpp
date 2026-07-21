#pragma once

#include <optional>
#include <vector>
#include <string>

class FileSystemHandler {
	public:
		static std::optional<std::vector<char>> read_file(const std::string& filepath);
};
