#pragma once

#include <sstream>

#include "API/Ark/Ark.h"

void SendChatMessageToAll(AShooterPlayerController* playerController, EChatSendMode::Type messageType, FString &message, UTexture2D *icon);
void SendChatMessageToAll(unsigned int senderId, FString &characterName, FString &steamName, FString &tribeName, EChatSendMode::Type sendMode, FString &message, UTexture2D *icon);

FString GetTribeName(AShooterPlayerController* playerController);

UTexture2D* loadTexture2D(const std::string path);
UTexture2D* findIconByPath(const std::string path);
std::string findIconForPlayer(AShooterPlayerController* playerController);