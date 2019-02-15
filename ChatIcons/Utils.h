#pragma once

#include <sstream>

#ifdef ARKAPI_GAME_ARK
#include <API/ARK/Ark.h>
#include <ArkPermissions.h>
#else
#include <API/Atlas/Atlas.h>
#include <AtlasPermissions.h>
#endif

#include <API/UE/UE.h>

int GetTribeId(AShooterPlayerController* playerController);
FString GetTribeRank(AShooterPlayerController* playerController);

UTexture2D* loadTexture2D(const std::string path);
UTexture2D* findIconByPath(const std::string path);
std::string findIconForMessage(FChatMessage msg);