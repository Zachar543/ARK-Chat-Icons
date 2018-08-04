#pragma once

#include "json.hpp"

#include "Utils.h"

class Plugin {
public:
	static Plugin& Get();

	nlohmann::json config;
	bool interceptGlobalChat;
	bool interceptTribeChat;
	bool interceptAllianceChat;
	bool interceptLocalChat;
	float localChatDistance;

	std::map<std::string, UTexture2D*> textureCache;

private:
	Plugin() = default;
	~Plugin() = default;
};
