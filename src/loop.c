#include "loop.h"
#include "minilib.h"
#include "timer.h"
#include "game.h"

void initGame() {
  DI;
  bios_disable_screen();
  initGraphics();
  initSound();
  initGameState();
  bios_enable_screen();
  EI;
}

void loop() {
  const bool is60hz = ((*(u_char*)0x002b) & 128) == 0;
  bool shouldTick;

  initTimerHook();
  initGame();
  while (true) {
    waitVBlank();
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
      DI;
      graphicsTick();
      EI;
      soundTick();
      gameTick();
      ++gameFrame;
      gameFrameSecFlag = false;
    }
    ++gameFrameMod60;
    ++gameFrameMod10;
  }
}
