#include "Utils.h"

#include <Logger/Logger.h>
#include <Permissions.h>

#include "Plugin.h"

void SendChatMessageToAll(AShooterPlayerController* playerController, EChatSendMode::Type messageType, FString &message, UTexture2D *icon) {
	unsigned int senderId;
	if (playerController) {
		AShooterCharacter* playerCharacter = playerController->GetPlayerCharacter();
		if (playerCharacter)
			senderId = playerCharacter->LinkedPlayerDataIDField();
	}

	FString characterName = ArkApi::IApiUtils::GetCharacterName(playerController);
	FString steamName = ArkApi::IApiUtils::GetSteamName(playerController);
	FString tribeName = GetTribeName(playerController);

	SendChatMessageToAll(
		senderId,
		characterName,
		steamName,
		tribeName,
		messageType,
		message,
		icon);
}
void SendChatMessageToAll(unsigned int senderId, FString &characterName, FString &steamName, FString &tribeName, EChatSendMode::Type sendMode, FString &message, UTexture2D *icon) {
	FChatMessage chatMessage = FChatMessage();
	chatMessage.SenderId = senderId;
	chatMessage.SenderName = characterName;
	chatMessage.SenderSteamName = steamName;
	chatMessage.SenderTribeName = tribeName;
	chatMessage.SendMode = sendMode;
	chatMessage.Message = message;
	chatMessage.SenderIcon = icon;

	FString logMessage = FString::Format("{0} ({1}): {2}", steamName.ToString(), characterName.ToString(), message.ToString());
	ArkApi::GetApiUtils().GetShooterGameMode()->PrintToServerGameLog(&logMessage, false);

	const TArray<TAutoWeakObjectPtr<APlayerController>>& playerControllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
	for (TWeakObjectPtr<APlayerController> playerController : playerControllers) {
		AShooterPlayerController* shooterPlayerController = static_cast<AShooterPlayerController*>(playerController.Get());
		shooterPlayerController->ClientChatMessage(chatMessage);
	}
}

FString GetTribeName(AShooterPlayerController* playerController) {
	AShooterPlayerState* playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState)
		return playerState->MyTribeDataField()->TribeNameField();

	return FString("");
}

UTexture2D* loadTexture2D(const std::string path) {
	auto& plugin = Plugin::Get();

	auto iter = plugin.textureCache.find(path);
	if (iter != plugin.textureCache.end()) {
		Log::GetLog()->debug("Utils::loadTexture2D(\"{}\") Texture Cache Hit", path.c_str());
		return iter->second;
	}

	auto result = reinterpret_cast<UTexture2D*>(Globals::StaticLoadObject(UTexture2D::StaticClass(), nullptr, ArkApi::Tools::ConvertToWideStr(path).c_str(), nullptr, 0, 0, true));

	Log::GetLog()->debug("Utils::loadTexture2D(\"{}\") Texture Cache Miss", path.c_str());
	plugin.textureCache.insert(std::pair<std::string, UTexture2D*>(path, result));


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