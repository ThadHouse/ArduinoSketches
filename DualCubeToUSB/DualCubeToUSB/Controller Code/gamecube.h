// gamecube.h
#ifndef _GAMECUBE_h
#define _GAMECUBE_h

#include "gamepad.h"

#define LEFT_DIFF 100
#define RIGHT_DIFF 90
#define SLIDER_DIFF 175
#define SLIDER_END 245

#ifdef __cplusplus
extern "C"{
#endif

	GCpad *gamecubeGetGamepad();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
