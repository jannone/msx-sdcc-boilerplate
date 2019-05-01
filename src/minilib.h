#ifndef __MINILIB__
#define __MINILIB__

#include "defs.h"

enum video_mode {
	mode_0 = 0x6C,
	mode_1 = 0x6F,
	mode_2 = 0x72,
	mode_3 = 0x75
};

typedef struct {
	u_char y;	///< Y position
	u_char x;	///< X position
	u_char handle;	///< internal vdp handle
	u_char color;	///< sprite color
} sprite_t;

/// sprite modes
enum sprite_mode {
	sprite_default = 0,
	sprite_scaled = 1,
	sprite_large = 2
};

void init();
void set_mode(u_int mode) __naked;
u_char get_stick(u_char id) __naked;
bool get_trigger(u_char id) __naked;
void psg_init();
u_char get_vdp(u_char reg);
void set_vdp(u_char reg, u_char value) __naked;
void vwrite(void *source, u_int dest, u_int count) __naked;
void vpoke(u_int addr, u_char value) __naked;
void vfill(u_int addr, u_char value, u_int count) __naked;
void set_color(u_char front, u_char back, u_char border);
void set_mangled_mode();
void set_sprite_mode(u_char mode);
void set_sprite_16(u_char handle, void *data);
void buffer_sprite_16(sprite_t *sp, const int x, const int y, const u_char handle, const u_char color);
void put_sprite_16(const u_char id, const int x, const int y, const u_char handle, const u_char color);
void enableScreen();
void disableScreen();
void clearSprites();
u_int random16();

// experimental
void vwrite_fast(void *source, u_int dest, u_char count) __naked;
void vfill_fast(u_int addr, u_char value, u_char count) __naked;
void vdp_set_address(u_int addr) __naked;
void vdp_send_value(u_char value, u_char count) __naked;
void vdp_send_data(void *source, u_char count) __naked;

#endif
