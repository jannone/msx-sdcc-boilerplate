#include "defs.h"
#include "minilib.h"

// Here you can add your static variables and definitions.
// Of course, you are free to create new files.

typedef struct {
  char x;
  char y;
} Star;

#define MAX_STARS 30

static Star stars[MAX_STARS];

// initGraphics: initialize your graphics mode here
// you can also load any graphics resources
void initGraphics() {
  set_color(15, 1, 1);
  set_mode(mode_1);
}

// initSound: initialize your sound driver here
// you can also load any sound and music resources
void initSound() {
}

// initGameState: initialize your game variables here
// you probably want to have a state to tell what is the current
// status of the game (introduction, menu, level, game over...)
// you can load additional resources, such as levels, enemy positions, etc
void initGameState() {
  char i;
  for (i=0; i<MAX_STARS; i++) {
    stars[i].x = get_random16() % 32;
    stars[i].y = get_random16() % 24;
  }
}

// gameTick: add game logic that runs at every frame
void gameTick() {
  char i;
  for (i=0; i<MAX_STARS; i++) {
    if (--stars[i].x < 0) {
      stars[i].x = 31;
      stars[i].y = get_random16() % 24;
    }
  }
}

// gameTick: render/update your game at every frame
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

// soundTick: emit sound at every frame
void soundTick() {
}
