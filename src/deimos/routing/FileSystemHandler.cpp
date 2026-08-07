#include "deimos/routing/FileSystemHandler.hpp"

#include <fstream>

std::optional<std::vector<char>> FileSystemHandler::read_file(const std::string& filepath) {
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		return std::nullopt;
	}

	std::streamsize size = file.tellg();

	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);

	if (file.read(buffer.data(), size)) {
		return buffer;
	}

	return std::nullopt;
}
