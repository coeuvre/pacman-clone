#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_game_bridge.h"

extern platform *Platform;

EXPORT void
UpdateAndRenderGame(platform *Platform);

#ifdef __cplusplus
}
#endif

#endif // GAME_H
