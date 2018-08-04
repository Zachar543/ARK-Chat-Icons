#pragma once

#include "API/Ark/Ark.h"

#include "json.hpp"

void SendChatMessageToAll(AShooterPlayerController* playerController, EChatSendMode::Type messageType, FString &message, UTexture2D *icon);

std::string GetPlayerName(AShooterPlayerController* playerController);
std::wstring GetPlayerCharacterName(AShooterPlayerController* playerController);
std::string GetTribeName(AShooterPlayerController* playerController);

std::string ToUTF8(const std::wstring &s);
std::wstring FromUTF8(const std::string &s);
std::wstring FromUTF16(const std::u16string &s);
std::u16string ToUTF16(const std::wstring &s);

UTexture2D* loadTexture2D(const std::string path);
UTexture2D* findIconByPath(const std::string path);

std::string findIconForPlayer(AShooterPlayerController* playerController);