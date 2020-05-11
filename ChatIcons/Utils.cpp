#include "Utils.h"

#include "Plugin.h"
#include <API/ARK/Ark.h>


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

			FString rankName;
			tribe->GetRankNameForPlayerID(&rankName, playerId);
			Log::GetLog()->debug("Utils::GetTribeRank() -> rankName={}", rankName.ToString());
			
			return rankName;
		}
	}

	return FString("");
}

UTexture2D* loadTexture2D(const std::string path) {
	auto& plugin = Plugin::Get();

	return reinterpret_cast<UTexture2D*>(Globals::StaticLoadObject(UTexture2D::StaticClass(), nullptr, ArkApi::Tools::ConvertToWideStr(path).c_str(), nullptr, 0, 0, true));
}
UTexture2D* findIconByPath(const std::string path) {
	if (path.length() == 0) return NULL;

	Log::GetLog()->debug("Utils::findIconByPath(\"{}\")", path.c_str());
	return loadTexture2D(path);
}
std::string findIconForMessage(FChatMessage msg) {
	auto& plugin = Plugin::Get();

	auto senderId = msg.SenderId;
	if (!senderId) return "";

	const uint64 steamId = ArkApi::GetApiUtils().GetSteamIDForPlayerID(senderId);
	if (!steamId) return "";

	AShooterPlayerController* player = ArkApi::GetApiUtils().FindPlayerFromSteamId(steamId);
	if (!player) return "";

	const uint64 tribeId = GetTribeId(player);
	const std::string tribeRank = GetTribeRank(player).ToString();

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