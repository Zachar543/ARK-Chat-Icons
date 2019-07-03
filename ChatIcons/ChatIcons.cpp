#include <windows.h>
#include <fstream>

#ifdef ARKAPI_GAME_ARK
#include <API/ARK/Ark.h>
#else
#include <API/Atlas/Atlas.h>
#endif

#include <API/UE/Math/ColorList.h>
#include <Logger/Logger.h>
#include "Plugin.h"

#ifdef ARKAPI_GAME_ARK
#pragma comment(lib, "ArkApi.lib")
#else
#pragma comment(lib, "AtlasApi.lib")
#endif

#pragma comment(lib, "Permissions.lib")

DECLARE_HOOK(AShooterPlayerController_ClientChatMessage, void, AShooterPlayerController*, FChatMessage);
void Hook_AShooterPlayerController_ClientChatMessage(AShooterPlayerController* _this, FChatMessage msg) {
	msg.SenderIcon = findIconByPath(findIconForMessage(msg));

	AShooterPlayerController_ClientChatMessage_original(_this, msg);
}

void loadConfig() {
	auto& plugin = Plugin::Get();

	try {
#ifdef ARKAPI_GAME_ARK
		const std::string path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/ChatIcons/config.json";
#else
		const std::string path = ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/ChatIcons/config.json";
#endif

		std::ifstream file{ path };
		if (!file.is_open()) {
			throw std::runtime_error("Failed to load config!");
		}

		file >> plugin.config;
		file.close();

		Log::GetLog()->set_level(spdlog::level::info);
		if (plugin.config.value("Debug", false))
			Log::GetLog()->set_level(spdlog::level::debug);

		plugin.steamIdIconMap.clear();
		auto steamIds = plugin.config.value("SteamIds", nlohmann::json::object());
		for (auto it = steamIds.begin(); it != steamIds.end(); ++it) {
			uint64 key = std::stoll(it.key());
			std::string value = it.value().get<std::string>();
			plugin.steamIdIconMap.insert(std::pair<uint64, std::string>(key, value));

			Log::GetLog()->debug("[Config::SteamIds] {}={}", key, value);
		}

		plugin.tribeIconMap.clear();
		auto tribeIds = plugin.config.value("TribeIds", nlohmann::json::object());
		for (auto it = tribeIds.begin(); it != tribeIds.end(); ++it) {
			uint64 key = std::stoll(it.key());
			auto value = it.value().get<std::map<std::string, std::string>>();
			plugin.tribeIconMap.insert(std::pair<uint64, std::map<std::string, std::string>>(key, value));

			for (auto tribeRank : value) {
				Log::GetLog()->debug("[Config::TribeIds] <{}> {}={}", key, tribeRank.first, tribeRank.second);
			}
		}

		plugin.permGroupIconMap.clear();
		auto groups = plugin.config.value("Groups", nlohmann::json::object());
		for (auto it = groups.begin(); it != groups.end(); ++it) {
			std::string key = it.key();
			std::string value = it.value().get<std::string>();
			plugin.permGroupIconMap.insert(std::pair<std::string, std::string>(key, value));

			Log::GetLog()->debug("[Config::Groups] {}={}", key, value);
		}
	}
	catch (const std::exception& error) {
		Log::GetLog()->error(error.what());
		throw;
	}
}
void reloadConfigConsoleCmd(APlayerController* playerController, FString*, bool) {
	loadConfig();

	AShooterPlayerController* shooterPlayer = static_cast<AShooterPlayerController*>(playerController);
	ArkApi::GetApiUtils().SendServerMessage(shooterPlayer, FColorList::Green, "Reloaded config");
}
void reloadConfigRconCmd(RCONClientConnection* conn, RCONPacket* packet, UWorld* world) {
	loadConfig();

	FString response = "Reloaded config";
	conn->SendMessageW(packet->Id, 0, &response);
}

void load() {
	Log::Get().Init("ChatIcons");

	loadConfig();

	ArkApi::GetHooks().SetHook("AShooterPlayerController.ClientChatMessage", &Hook_AShooterPlayerController_ClientChatMessage, &AShooterPlayerController_ClientChatMessage_original);
	ArkApi::GetCommands().AddConsoleCommand("ChatIcons.Reload", &reloadConfigConsoleCmd);
	ArkApi::GetCommands().AddRconCommand("ChatIcons.Reload", &reloadConfigRconCmd);
}
void unload() {
 	ArkApi::GetHooks().DisableHook("AShooterPlayerController.ClientChatMessage", &Hook_AShooterPlayerController_ClientChatMessage);
	ArkApi::GetCommands().RemoveConsoleCommand("ChatIcons.Reload");
	ArkApi::GetCommands().RemoveRconCommand("ChatIcons.Reload");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD event, LPVOID lpReserved) {
	switch (event) {
		case DLL_PROCESS_ATTACH:
			load();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			unload();
			break;
	}

	return TRUE;
}
