#include "defs.h"
#include "minilib.h"
#include "timer.h"

// globals
static u_char stick;
static u_int gameFrame = 0;
static u_char gameFrameMod60 = 0, gameFrameMod10 = 0;
static bool gameFrameSecFlag;

void initGraphics() {
  // implement this
	set_color(15, 1, 1);
	set_mode(mode_1);
}

void initSoundDriver() {
  // implement this
}

void initGameState() {
  // implement this
}

void gameTick() {
  // implement this
  const char* text = "Hello world!";
  vwrite_fast(text, 6144, 12);
}

void soundDriverTick() {
  // implement this
}

void initGame() {
  initGraphics();
  initSoundDriver();
  initGameState();
}

void game() {
  const bool is60hz = ((*(u_char*)0x002b) & 128) == 0;
  bool shouldTick;

  initTimerHook();
  initGame();
  while (true) {
    waitVBlank();
    stick = get_stick(0);

    if (gameFrameMod60 == 60) {
      gameFrameMod60 = 0;
      gameFrameSecFlag = true;
    }
    shouldTick = true;
    if (gameFrameMod10 == 10) {
      gameFrameMod10 = 0;
      if (is60hz) {
        shouldTick = false;
      }
    }
    if (shouldTick) {
      gameTick();
      soundDriverTick();
      ++gameFrame;
      gameFrameSecFlag = false;
    }
    ++gameFrameMod60;
    ++gameFrameMod10;
  }
}
