#pragma once

#include "json.hpp"

#include "Utils.h"

class Plugin {
public:
	static Plugin& Get();

	nlohmann::json config;
	std::map<std::string, UTexture2D*> textureCache;

private:
	Plugin() = default;
	~Plugin() = default;
};
