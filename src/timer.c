#include "timer.h"

static unsigned char prevTick, curTick;

void waitVBlank() {
  __asm 
  ei
  __endasm;
  while (prevTick == curTick) {
    curTick = *(unsigned char*)(0xFC9E);
  }
  prevTick = curTick;
}

void initTimerHook() __naked {
  __asm 

  LD	HL, #HookSetup	
  LD	DE, #0xFD9A ; HKEYI
  LD	BC, #7
  LDIR			
  LD    A,(#0x0006)   ; A = base VDP *READ* port from BIOS
  INC   A             ; A = register port
  LD    (#0x0FD9B), A ; overwrites port number of "IN A, (#0)"
  RET

HookSetup:
  IN  A, (#0) ; port 00H as placeholder, will be mutated to port from BIOS later on
  LD  HL, #0x0FC9E  ; increments "TIME" BASIC variable
  INC	(HL)
  RET

  __endasm;
}
