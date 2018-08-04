#include <windows.h>
#include <fstream>

#include <API/ARK/Ark.h>
#include <API/UE/Math/ColorList.h>
#include <Logger/Logger.h>

#include "Plugin.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")

bool onChatMessage(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type messageType, bool spam, bool command) {
	if (spam || command) return false;

	if (playerController && message) {
		auto icon = findIconByPath(findIconForPlayer(playerController));

		SendChatMessageToAll(playerController, messageType, *message, icon);
		
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
	
	ArkApi::GetCommands().AddOnChatMessageCallback("ArkChatIcon::onChatMessage", &onChatMessage);
	ArkApi::GetCommands().AddConsoleCommand("ArkChatIcon.Reload", &reloadConfigCmd);
}
void unload() {
	ArkApi::GetCommands().RemoveOnChatMessageCallback("ArkChatIcon::onChatMessage");
	ArkApi::GetCommands().RemoveConsoleCommand("ArkChatIcon.Reload");
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
