#pragma once

#include "Utils.h"

class Plugin {
public:
	nlohmann::json config;
	
	static Plugin& Get();

	Plugin(const Plugin&) = delete;
	Plugin(Plugin&&) = delete;
	Plugin& operator=(const Plugin&) = delete;
	Plugin& operator=(Plugin&&) = delete;

private:
	Plugin() = default;
	~Plugin() = default;
};
