#ifndef __LOOP__
#define __LOOP__

#include "defs.h"

// globals (for performance reasons)
static u_int gameFrame = 0;
static u_char gameFrameMod60 = 0, gameFrameMod10 = 0;
static bool gameFrameSecFlag;

void loop();

#endif
