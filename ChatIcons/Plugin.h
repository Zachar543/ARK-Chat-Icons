#pragma once

#include "json.hpp"

#include "Utils.h"

class Plugin {
public:
	static Plugin& Get();

	nlohmann::json config;

	std::map<uint64, std::string> steamIdIconMap;
	std::map<uint64, std::map<std::string, std::string>> tribeIconMap;
	std::map<std::string, std::string> permGroupIconMap;

private:
	Plugin() = default;
	~Plugin() = default;
};
