#include "Utils.h"

#include <Logger/Logger.h>
#include <Permissions.h>

#include "Plugin.h"

void SendChatMessageToAll(AShooterPlayerController* playerController, EChatSendMode::Type messageType, FString &message, UTexture2D *icon) {
	uint64 characterId = ArkApi::IApiUtils::GetPlayerID(playerController);
	FString characterName = ArkApi::IApiUtils::GetCharacterName(playerController);
	FString steamName = ArkApi::IApiUtils::GetSteamName(playerController);
	FString tribeName = GetTribeName(playerController);
	int senderTeamIndex = GetTeamId(playerController);

	SendChatMessageToAll(
		playerController,
		characterId,
		characterName,
		steamName,
		tribeName,
		messageType,
		message,
		icon,
		senderTeamIndex);
}
void SendChatMessageToAll(AShooterPlayerController* playerController, unsigned int characterId, FString &characterName, FString &steamName, FString &tribeName, EChatSendMode::Type sendMode, FString &message, UTexture2D *icon, int senderTeamIndex) {
	auto& plugin = Plugin::Get();

	FChatMessage chatMessage = FChatMessage();
	chatMessage.SenderId = characterId;
	chatMessage.SenderName = characterName;
	chatMessage.SenderSteamName = steamName;
	chatMessage.SenderTribeName = tribeName;
	chatMessage.SendMode = sendMode;
	chatMessage.ChatType = GetChatType(sendMode);
	chatMessage.Message = message;
	chatMessage.SenderIcon = icon;
	chatMessage.SenderTeamIndex = senderTeamIndex;

	if ((sendMode == EChatSendMode::GlobalChat && plugin.logGlobalChat) ||
		(sendMode == EChatSendMode::GlobalTribeChat && plugin.logTribeChat) || 
		(sendMode == EChatSendMode::AllianceChat && plugin.logAllianceChat) || 
		(sendMode == EChatSendMode::LocalChat && plugin.logLocalChat)) {
		FString logMessage = FString::Format("{0} ({1}): {2}", steamName.ToString(), characterName.ToString(), message.ToString());
		ArkApi::GetApiUtils().GetShooterGameMode()->PrintToServerGameLog(&logMessage, false);
	}
	
	const TArray<TAutoWeakObjectPtr<APlayerController>>& playerControllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
	for (TWeakObjectPtr<APlayerController> otherPlayerController : playerControllers) {
		AShooterPlayerController* shooterPlayerController = static_cast<AShooterPlayerController*>(otherPlayerController.Get());
		if (ShouldPlayerGetMessage(shooterPlayerController, playerController, sendMode))
			shooterPlayerController->ClientChatMessage(chatMessage);
	}
}

bool ShouldPlayerGetMessage(AShooterPlayerController* sender, AShooterPlayerController* player, EChatSendMode::Type sendMode) {
	auto& plugin = Plugin::Get();

	if (sender == player) return true;
	if (sendMode == EChatSendMode::AllianceChat){
		auto senderAlliances = GetTribeAlliances(sender);
		auto playerAlliances = GetTribeAlliances(player);
		for (auto allianceA : senderAlliances) {
			for (auto allianceB : playerAlliances) {
				if (allianceA.AllianceID == allianceB.AllianceID)
					return true;
			}
		}
		return false;
	}
	if (sendMode == EChatSendMode::GlobalTribeChat) {
		auto senderTribeId = GetTribeId(sender);
		auto playerTribeId = GetTribeId(player);
		return (senderTribeId == playerTribeId);
	}
	if (sendMode == EChatSendMode::GlobalChat) return true;
	if (sendMode == EChatSendMode::LocalChat) {
		if (!sender || !player) return false;
		FVector senderPos = ArkApi::GetApiUtils().GetPosition(sender);
		FVector playerPos = ArkApi::GetApiUtils().GetPosition(player);
		float dist = FVector::Dist(senderPos, playerPos);
		Log::GetLog()->debug("Utils::ShouldPlayerGetMessage(): Dist = {}", std::to_string(dist));
		return (dist <= plugin.localChatDistance);
	}
	//else if (sendMode == EChatSendMode::MAX) return true; // TODO: MAX?

	return true;
}

EChatType::Type GetChatType(EChatSendMode::Type sendMode) {
	switch (sendMode) {
	case EChatSendMode::GlobalChat:
		return EChatType::GlobalChat;
	case EChatSendMode::GlobalTribeChat:
		return EChatType::GlobalTribeChat;
	case EChatSendMode::LocalChat:
		return EChatType::ProximityChat;
	case EChatSendMode::AllianceChat:
		return EChatType::AllianceChat;
	case EChatSendMode::MAX:
		return EChatType::MAX;
	default:
		return EChatType::GlobalChat;
	}
}

int GetTeamId(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState)
		return playerState->TargetingTeamField();

	return -1;
}
FString GetTribeName(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe)
			return tribe->TribeNameField();
	}


	return FString("");
}
int GetTribeId(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe)
			return tribe->TribeIDField();
	}

	return -1;
}
FString GetTribeRank(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe) {
			uint64 playerId = ArkApi::IApiUtils::GetPlayerID(playerController);

			auto tribeRanks = tribe->TribeRankGroupsField();
			if (tribeRanks.Num() > 0) {
				auto tribeRank = tribeRanks[0];

				FTribeRankGroup outRank(tribeRank);
				bool result = tribe->GetTribeRankGroupForPlayer(playerId, &outRank);

				Log::GetLog()->debug("Utils::GetTribeRank() -> result={} outRank={}", std::to_string(result), outRank.RankGroupName.ToString());
				return outRank.RankGroupName;
			}
		}
	}

	return FString("");
}
TArray<FTribeAlliance> GetTribeAlliances(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe)
			return tribe->TribeAlliancesField();
	}

	return TArray<FTribeAlliance>();
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

	const uint64 steamId = ArkApi::IApiUtils::GetSteamIdFromController(playerController);
	const uint64 tribeId = GetTribeId(playerController);
	const std::string tribeRank = GetTribeRank(playerController).ToString();

	// Check Steam Id
	Log::GetLog()->debug("Utils::findIconForPlayer() -> steamIdStr={}", steamId);
	auto steamIdIter = plugin.steamIdIconMap.find(steamId);
	if (steamIdIter != plugin.steamIdIconMap.end()) {
		return steamIdIter->second;
	}

	// Check Tribe Id
	Log::GetLog()->debug("Utils::findIconForPlayer() -> tribeId={} tribeRank={}", tribeId, tribeRank);
	auto tribeIdIter = plugin.tribeIconMap.find(tribeId);
	if (tribeIdIter != plugin.tribeIconMap.end()) {
		auto tribeRankIter = tribeIdIter->second.find(tribeRank);
		if (tribeRankIter != tribeIdIter->second.end()) {
			return tribeRankIter->second;
		}
	}

	// Check Permission Groups
	for (FString group : Permissions::GetPlayerGroups(steamId)) {
		Log::GetLog()->debug("Utils::findIconForPlayer() -> group={}", group.ToString());
		auto groupIter = plugin.permGroupIconMap.find(group.ToString());
		if (groupIter != plugin.permGroupIconMap.end()) {
			return groupIter->second;
		}
	}

	return "";
}