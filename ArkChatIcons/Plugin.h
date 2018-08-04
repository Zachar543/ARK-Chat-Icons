#pragma once

#include "json.hpp"

#include "Utils.h"

class Plugin {
public:
	static Plugin& Get();

	nlohmann::json config;

private:
	Plugin() = default;
	~Plugin() = default;
};
