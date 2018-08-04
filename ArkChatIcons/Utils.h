#pragma once

#include <sstream>

#include "API/Ark/Ark.h"

void SendChatMessageToAll(AShooterPlayerController* playerController, EChatSendMode::Type messageType, FString &message, UTexture2D *icon);
void SendChatMessageToAll(AShooterPlayerController* playerController, unsigned int senderId, FString &characterName, FString &steamName, FString &tribeName, EChatSendMode::Type sendMode, FString &message, UTexture2D *icon, int senderTeamIndex);

bool ShouldPlayerGetMessage(AShooterPlayerController* sender, AShooterPlayerController* player, EChatSendMode::Type sendMode);

EChatType::Type GetChatType(EChatSendMode::Type sendMode);
int GetTeamId(AShooterPlayerController* playerController);
FString GetTribeName(AShooterPlayerController* playerController);
int GetTribeId(AShooterPlayerController* playerController);
TArray<FTribeAlliance> GetTribeAlliances(AShooterPlayerController* playerController);

UTexture2D* loadTexture2D(const std::string path);
UTexture2D* findIconByPath(const std::string path);
std::string findIconForPlayer(AShooterPlayerController* playerController);