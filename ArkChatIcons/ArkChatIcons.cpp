#include <windows.h>
#include <fstream>

#include <API/ARK/Ark.h>
#include <API/UE/Math/ColorList.h>
#include <Logger/Logger.h>

#include "Plugin.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")

bool onChatMessage(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type sendMode, bool spam, bool command) {
	if (spam || command) return false;

	auto& plugin = Plugin::Get();

	// Disable handling of a few SendModes
	if (sendMode == EChatSendMode::MAX) return false;

	if (sendMode == EChatSendMode::GlobalChat && !plugin.interceptGlobalChat) return false;
	if (sendMode == EChatSendMode::GlobalTribeChat && !plugin.interceptTribeChat) return false;
	if (sendMode == EChatSendMode::AllianceChat && !plugin.interceptAllianceChat) return false;
	if (sendMode == EChatSendMode::LocalChat && !plugin.interceptLocalChat) return false;

	if (playerController && message) {
		auto icon = findIconByPath(findIconForPlayer(playerController));

		SendChatMessageToAll(playerController, sendMode, *message, icon);
		
		return true;
	}
	
	return false;
}

void loadConfig() {
	auto& plugin = Plugin::Get();

	try {
		const std::string path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/ArkChatIcons/config.json";

		std::ifstream file{ path };
		if (!file.is_open()) {
			throw std::runtime_error("Failed to load config!");
		}

		file >> plugin.config;
		file.close();

		plugin.interceptGlobalChat = plugin.config.value("InterceptGlobalChat", true);
		plugin.interceptTribeChat = plugin.config.value("InterceptTribeChat", true);
		plugin.interceptAllianceChat = plugin.config.value("InterceptAllianceChat", true);
		plugin.interceptLocalChat = plugin.config.value("InterceptLocalChat", true);
		plugin.localChatDistance = plugin.config.value("LocalChatDistance", 1000);
	}
	catch (const std::exception& error) {
		Log::GetLog()->error(error.what());
		throw;
	}
}
void reloadConfigCmd(APlayerController* playerController, FString*, bool) {
	AShooterPlayerController* shooterPlayer = static_cast<AShooterPlayerController*>(playerController);

	loadConfig();

	ArkApi::GetApiUtils().SendServerMessage(shooterPlayer, FColorList::Green, "Reloaded config");
}

void load() {
	Log::Get().Init("ArkChatIcons");

	loadConfig();
	
	ArkApi::GetCommands().AddOnChatMessageCallback("ArkChatIcons::onChatMessage", &onChatMessage);
	ArkApi::GetCommands().AddConsoleCommand("ArkChatIcons.Reload", &reloadConfigCmd);
}
void unload() {
	ArkApi::GetCommands().RemoveOnChatMessageCallback("ArkChatIcons::onChatMessage");
	ArkApi::GetCommands().RemoveConsoleCommand("ArkChatIcons.Reload");
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
