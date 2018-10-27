#pragma once

#include <sstream>

#include <API/ARK/Ark.h>
#include <API/UE/UE.h>

int GetTribeId(AShooterPlayerController* playerController);
FString GetTribeRank(AShooterPlayerController* playerController);

UTexture2D* loadTexture2D(const std::string path);
UTexture2D* findIconByPath(const std::string path);
std::string findIconForMessage(FChatMessage msg);