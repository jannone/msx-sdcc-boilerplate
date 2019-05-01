#ifndef __DEFS__
#define __DEFS__

#ifdef __SDCC
	#define DI __asm di __endasm
	#define EI __asm ei __endasm
#else
	#define DI 
	#define EI 
	#define __naked
#endif

#define u_int	unsigned int
#define u_char unsigned char
#define bool char
#define true 1
#define false	0
#ifndef NULL
	#define NULL 0
#endif

#endif
