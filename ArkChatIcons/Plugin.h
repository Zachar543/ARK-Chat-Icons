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
	bool logGlobalChat;
	bool logTribeChat;
	bool logAllianceChat;
	bool logLocalChat;
	float localChatDistance;

	std::map<uint64, std::string> steamIdIconMap;
	std::map<uint64, std::map<std::string, std::string>> tribeIconMap;
	std::map<std::string, std::string> permGroupIconMap;

	std::map<std::string, UTexture2D*> textureCache;

private:
	Plugin() = default;
	~Plugin() = default;
};
