#include <windows.h>
#include <fstream>
#include <chrono>
#include <random>
#include <codecvt>
#include <Logger/Logger.h>
#include <Permissions.h>

#include "Plugin.h"
#include "Utils.h"

void _SendChatMessageToAll(const unsigned int senderId, const FString &senderName, const FString &senderSteamName, const FString &senderTribeName, EChatSendMode::Type messageType, const FString &message, UTexture2D *senderIcon) {
	FChatMessage chatMessage = FChatMessage();
	chatMessage.SenderId = senderId;
	chatMessage.SenderName = senderName;
	chatMessage.SenderSteamName = senderSteamName;
	chatMessage.SenderTribeName = senderTribeName;
	chatMessage.SendMode = messageType;
	chatMessage.Message = message;
	chatMessage.SenderIcon = senderIcon;

	auto world = ArkApi::GetApiUtils().GetWorld();
	if (!world) return;

	Log::GetLog()->info("{} ({}): {}", 
		ArkApi::Tools::ConvertToAnsiStr(*senderSteamName), 
		ArkApi::Tools::ConvertToAnsiStr(*senderName), 
		ArkApi::Tools::ConvertToAnsiStr(*message));

	const auto& playerControllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
	for (TWeakObjectPtr<APlayerController> playerController : playerControllers) {
		AShooterPlayerController* shooterPC = static_cast<AShooterPlayerController*>(playerController.Get());
		shooterPC->ClientChatMessage(chatMessage);
	}
}
void SendChatMessageToAll(AShooterPlayerController* playerController, EChatSendMode::Type messageType, FString &message, UTexture2D *icon) {
	unsigned int senderId;
	if (playerController) {
		AShooterCharacter* playerCharacter = playerController->GetPlayerCharacter();
		if (playerCharacter) 
			senderId = playerCharacter->LinkedPlayerDataIDField();
	}
	
	auto name = FString(ToUTF8(GetPlayerCharacterName(playerController)).c_str());
	//auto nameWithTag = FString(ArkApi::Tools::ConvertToWideStr("[{Rank}] {Name}").c_str());
	//nameWithTag.ReplaceInline(L"{Name}", *name);
	//nameWithTag.ReplaceInline(L"{Rank}", L"M");
	//name = nameWithTag;

	_SendChatMessageToAll(
		senderId,
		name,
		FromUTF8(GetPlayerName(playerController)).c_str(),
		FromUTF8(GetTribeName(playerController)).c_str(),
		messageType,
		message,
		icon);
}

std::string GetPlayerName(AShooterPlayerController* playerController)
{
	std::string playerName;

	auto playerState = playerController->PlayerStateField();
	if (playerState) 
		playerName = ToUTF8(*playerState->PlayerNameField());

	return playerName;
}
std::wstring GetPlayerCharacterName(AShooterPlayerController* playerController) {
	auto characterName = ArkApi::GetApiUtils().GetCharacterName(playerController);

	return *characterName;
}
std::string GetTribeName(AShooterPlayerController* playerController) {
	std::string tribeName;

	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribeData = playerState->MyTribeDataField();
		tribeName = tribeData->TribeNameField().ToString();
	}

	return tribeName;
}

std::wstring FromUTF8(const std::string &s) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
	std::wstring wstr = conv1.from_bytes(s);
	return wstr;
}
std::string ToUTF8(const std::wstring &s) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
	std::string u8str = conv1.to_bytes(s);
	return u8str;
}

std::wstring FromUTF16(const std::u16string &s) {
	return std::wstring(s.begin(), s.end());
}
std::u16string ToUTF16(const std::wstring &s) {
	return std::u16string(s.begin(), s.end());
}

UTexture2D* loadTexture2D(const std::string path) {
	auto result = reinterpret_cast<UTexture2D*>(Globals::StaticLoadObject(UTexture2D::StaticClass(), nullptr, ArkApi::Tools::ConvertToWideStr(path).c_str(), nullptr, 0, 0, true));
	Log::GetLog()->debug("Utils::loadTexture2D(\"{}\") -> {}", path.c_str(), std::to_string((int)result));

	return result;
}
UTexture2D* findIconByPath(const std::string path) {
	if (path.length() == 0) return NULL;

	Log::GetLog()->debug("Utils::findIconByPath(\"{}\")", path.c_str());
	return loadTexture2D(path);
}

std::string findIconForPlayer(AShooterPlayerController* playerController) {
	auto& plugin = Plugin::Get();

	const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(playerController);
	const std::string steamIdStr = std::to_string(steamId);

	Log::GetLog()->debug("Utils::findIconForPlayer() -> steamIdStr: {}", steamIdStr);

	auto steamIds = plugin.config.value("SteamIds", nlohmann::json::object());
	std::string steamIdPath = steamIds.value(steamIdStr, "");
	if (steamIdPath.length() > 0)
		return steamIdPath;

	auto groups = plugin.config.value("Groups", nlohmann::json::object());
	for (FString group : Permissions::GetPlayerGroups(steamId)) {
		Log::GetLog()->debug("Utils::findIconForPlayer() -> group: {}", group.ToString());

		std::string path = groups.value(group.ToString(), "");
		if (path.length() > 0)
			return path;
	}

	return "";
}