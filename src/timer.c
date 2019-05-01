#include "defs.h"
#include "timer.h"

static unsigned char timeTick;

void waitVBlank() {
  while (timeTick == *(unsigned char*)(0xFC9E)) {
    // busy-waiting
    ;
  }
  timeTick = *(unsigned char*)(0xFC9E);
}

void initTimerHook() __naked {
  __asm 

  LD	HL, #HookSetup	
  LD	DE, #0xFD9F ; HTIMI
  LD	BC, #7
  LDIR			
  LD    A,(#0x0006)   ; A = base VDP *READ* port from BIOS
  INC   A             ; A = register port
  LD    (#0x0FDA0), A ; overwrites port number of "IN A, (#0)"
  RET

HookSetup:
  IN  A, (#0) ; port 00H as placeholder, will be mutated to port from BIOS later on
  LD  HL, #0x0FC9E  ; increments "TIME" BASIC variable
  INC	(HL)
  RET

  __endasm;
}
