#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_game_bridge.h"

extern platform *GlobalPlatform;

EXPORT GAME_INIT(Init);
EXPORT GAME_UPDATE(Update);
EXPORT GAME_RENDER(Render);

#ifdef __cplusplus
}
#endif

#endif // GAME_H
