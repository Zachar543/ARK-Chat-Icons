#include "Utils.h"

#include "Plugin.h"
#include <API/ARK/Ark.h>

FString getRankForPlayer(FTribeData* tribe, uint64 playerId, const TArray<FTribeRankGroup>::ElementType& tribeRank) {
	FTribeRankGroup outRank(tribeRank);
	bool result = tribe->GetTribeRankGroupForPlayer(playerId, &outRank);

#ifdef ARKAPI_GAME_ARK
	FString rankGroupName = outRank.RankGroupName;
#else
	FString rankGroupName = outRank.RankGroupNameField();
#endif

	Log::GetLog()->debug("Utils::GetTribeRank() -> result={} outRank={}", std::to_string(result), rankGroupName.ToString());
	return rankGroupName;
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

FString getRankForPlayer(const TArray<FTribeRankGroup>::ElementType& player_id, uint64 tribe, FTribeData* out_rank, FTribeRankGroup* tribe_rank_group);

FString GetTribeRank(AShooterPlayerController* playerController) {
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState) {
		auto tribe = playerState->MyTribeDataField();
		if (tribe) {
			uint64 playerId = ArkApi::IApiUtils::GetPlayerID(playerController);
			auto tribeRanks = tribe->TribeRankGroupsField();
			if (tribeRanks.Num() > 0) {
				auto tribeRank = tribeRanks[0];
				return getRankForPlayer(tribe, playerId, tribeRank);
			}
		}
	}

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
	// plugin.textureCache.insert(std::pair<std::string, UTexture2D*>(path, result));

	return result;
}
UTexture2D* findIconByPath(const std::string path) {
	if (path.length() == 0) return NULL;

	Log::GetLog()->debug("Utils::findIconByPath(\"{}\")", path.c_str());
	return loadTexture2D(path);
}
std::string findIconForMessage(FChatMessage msg) {
	auto& plugin = Plugin::Get();

	// Log::GetLog()->debug("SenderName: {}", msg.SenderName.ToString());
	// Log::GetLog()->debug("SenderSteamName: {}", msg.SenderSteamName.ToString());
	// Log::GetLog()->debug("SenderTribeName: {}", msg.SenderTribeName.ToString());
	// Log::GetLog()->debug("SenderId: {}", std::to_string(msg.SenderId));
	// Log::GetLog()->debug("UserId: {}", msg.UserId.ToString());
	// Log::GetLog()->debug("Message: {}", msg.Message.ToString());

	uint64 steamId;
	auto senderId = msg.SenderId;
	Log::GetLog()->debug("senderId={}", std::to_string(senderId));
	if (!senderId) {
		steamId = std::stoull(msg.UserId.ToString());
	}
	else {
		steamId = ArkApi::GetApiUtils().GetSteamIDForPlayerID(senderId);
	}

	Log::GetLog()->debug("steamId={}", std::to_string(steamId));
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