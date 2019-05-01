#include "defs.h"
#include "minilib.h"
#include "timer.h"

// globals
static u_int gameFrame = 0;
static u_char gameFrameMod60 = 0, gameFrameMod10 = 0;
static bool gameFrameSecFlag;

// custom stuff
// implement this

typedef struct {
  char x;
  char y;
} Star;

#define MAX_STARS 30

static Star stars[MAX_STARS];

void initGraphics() {
  // implement this
  set_color(15, 1, 1);
  set_mode(mode_1);
}

void initSound() {
  // implement this
}

void initGameState() {
  // implement this
  char i;
  for (i=0; i<MAX_STARS; i++) {
    stars[i].x = get_random16() % 32;
    stars[i].y = get_random16() % 24;
  }
}

void gameTick() {
  // implement this
  char i;
  for (i=0; i<MAX_STARS; i++) {
    if (--stars[i].x < 0) {
      stars[i].x = 31;
      stars[i].y = get_random16() % 24;
    }
  }
}

void graphicsTick() {
  // implement this
  char i;
  u_int addr;
  vdp_set_address(6144);
  vdp_send_value(' ', 0);
  vdp_send_value(' ', 0);
  vdp_send_value(' ', 0);
  for (i=0; i<MAX_STARS; i++) {
    addr = 6144 + stars[i].y * 32 + stars[i].x;
    vdp_set_address(addr);
    vdp_send_value('.', 1);
  }
}

void soundTick() {
  // implement this
}

void initGame() {
  DI;
  bios_disable_screen();
  initGraphics();
  initSound();
  initGameState();
  bios_enable_screen();
  EI;
}

void game() {
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
