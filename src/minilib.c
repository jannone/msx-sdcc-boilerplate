#include "minilib.h"

#pragma disable_warning 85

#define MODE2_ATTR (8192)

const u_char st_dir[] = {
  0x00, // 0
  0x01, // 1
  0x03, // 2
  0x02, // 3
  0x06, // 4
  0x04, // 5
  0x0C, // 6
  0x08, // 7
  0x09  // 8
};

void relocate_callbios_from_rom_to_ram() __naked {
__asm

callbios_ram_1      .equ #callbios_rom_1 - #callbios_rom_start + #callbios
callbios_rom_length .equ #callbios_rom_end - #callbios_rom_start

  push af
  push bc
  push de
  push hl
  ld hl, #callbios_rom_start
  ld de, #callbios
  ld bc, #callbios_rom_length
  ldir
  pop hl
  pop de
  pop bc
  pop af
  ret
  
callbios_rom_start:
  ld      (callbios_ram_1), ix
  rst     #0x30
callbios_rom_0:
  .db    0
callbios_rom_1:
  .dw    0
  ret
callbios_rom_end:
  
.area   _DATA
callbios:
  .ds #callbios_rom_length
.area   _CODE           
__endasm;
}

void init() {
  relocate_callbios_from_rom_to_ram();
}

void set_mode(u_int mode) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	push bc
	ld c, 4(ix)
	ld b, 5(ix)
	push bc
	pop ix
	call callbios
	pop bc

	pop ix           ;epilogue
	ret
                            
__endasm;
}

u_char get_stick(u_char id) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_gtstck .equ 0x00D5

	push hl
	push de
	push bc
	push ix

	ld a, 4(ix)
	ld ix, #f_gtstck
	call callbios

	pop ix
	pop bc
	pop de
	pop hl
	
	ld l, a

	pop ix           ;epilogue
	ret

__endasm;
}

bool get_trigger(u_char id) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_gttrig .equ 0x00D8

	push ix
	ld a, 4(ix)

	ld ix, #f_gttrig
	call callbios
	pop ix
	ld l, a

	pop ix           ;epilogue
	ret

__endasm;
}

void psg_init() {
__asm
	f_gicini .equ 0x0090

	push ix
	ld ix, #f_gicini
	call callbios
	pop ix
__endasm;
}

u_char get_vdp(u_char reg)
{
	return *(u_char *)(0xF3DF + reg);
}

void set_vdp(u_char reg, u_char value) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_wrtvdp .equ #0x0047

	push bc

	ld c, 4(ix)
	ld b, 5(ix)

	ld ix, #f_wrtvdp
	call callbios

	pop bc

	pop ix           ;epilogue
	ret

__endasm;
}

void vwrite(void *source, u_int dest, u_int count) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_ldirvm .equ 0x0005C

	push hl
	push bc
	push de
	push ix

	ld l, 4(ix)
	ld h, 5(ix)
	ld e, 6(ix)
	ld d, 7(ix)
	ld c, 8(ix)
	ld b, 9(ix)

	ld ix, #f_ldirvm
	call callbios

	pop ix
	pop de
	pop bc
	pop hl

	pop ix           ;epilogue
	ret

__endasm;
}

void vpoke(u_int addr, u_char value) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_wrtvrm .equ #0x004D

	p_vdp_data .equ #0x98
	p_vdp_cmd  .equ #0x99

	; enter vdp address pointer

	ld a, 4(ix)
	out (p_vdp_cmd), a
	ld a, 5(ix)
	and #0x3f
	or  #0x40
	out (p_vdp_cmd), a

	; enter data

	ld a, 6(ix)
	out (p_vdp_data), a

	pop ix           ;epilogue
	ret
		
__endasm;
}

void vfill(u_int addr, u_char value, u_int count) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_filvrm .equ #0x0056

	push af
	push bc
	push hl

	ld l, 4(ix)
	ld h, 5(ix)
	ld c, 7(ix)
	ld b, 8(ix)
	ld a, 6(ix)

	ld ix, #f_filvrm
	call callbios

	pop bc
	pop hl
	pop af

	pop ix           ;epilogue
	ret
                            
__endasm;
}

void set_color(u_char front, u_char back, u_char border) {
	*(u_char*)0xf3e9 = front;
	*(u_char*)0xf3ea = back;
	*(u_char*)0xf3eb = border;
__asm
	f_chgclr .equ #0x0062

	push ix
	ld ix, #f_chgclr
	call callbios
	pop ix	
__endasm;
}

void set_mangled_mode()
{
	set_mode(mode_1);
	set_mode(0x7E);
	vwrite((void *)0x1BBF, 0x0800, 0x800);
	vwrite((void *)0x1BBF, 0x1000, 0x800);
	vfill(MODE2_ATTR, 0xF0, 0x17FF);
	vfill(0xFF8, 0xFF, 8);
	vfill(0x17F8, 0xFF, 8);
}

void set_sprite_mode(u_char mode)
{
	u_char m = get_vdp(1);
	set_vdp(1, (m & 0xFC) | mode);
}

void set_sprite_16(u_char handle, void *data)
{
	vwrite(data, 14336 + (handle << 5), 32);
}

void buffer_sprite_16(sprite_t *sp, const int x, const int y, const u_char handle, const u_char color)
{
	if (x < 0) {
  	sp->x = x + 32;
    sp->color = color | 128;
	} else {
  	sp->x = x;
    sp->color = color;
  }
	sp->y = y - 1;
	sp->handle = (handle << 2);
}

void put_sprite_16(const u_char id, const int x, const int y, const u_char handle, const u_char color)
{
	sprite_t sp;
  buffer_sprite_16(&sp, x, y, handle, color);
  vwrite(&sp, 6912 + (id << 2), 4);
}

void vwrite_fast(void *source, u_int dest, u_char count) __naked {
__asm 

	pop af 
	pop hl 
	pop de 
	pop bc 
	push bc
	push de 
	push hl
	push af 

	ld a, c 
	ld b, a

	; get VDP base port
	ld a, (#0x0007)
	inc a
	ld c, a

	; set VRAM address
	out	(c), e
	set 6, d
	out	(c), d

	; prepare VDP data port (for OUTI)
	; the next instructions provide the correct timing for VDP as well
	dec c

	nop

vwrite_fast_loop:
	; must not use OTIR because of MSX1 VDP timings
	outi
	jp nz, vwrite_fast_loop
	ret

__endasm;	
}

void vfill_fast(u_int addr, u_char value, u_char count) __naked {
__asm 

	pop af 
	pop de 
	pop hl
	push hl
	push de
	push af 

	ld a, h
	ld b, a

	; get VDP base port
	ld a, (#0x0007)
	inc a
	ld c, a

	; set VRAM address
	out	(c), e
	set 6, d
	out	(c), d

	; prepare VDP data port (for OUT)
	; the next instructions provide the correct timing for VDP as well
	dec c

	nop

vfill_fast_loop:
	out (c), l
	dec b
	jp nz, vfill_fast_loop
	ret

__endasm;	
}

void vdp_set_address(u_int addr) __naked {
__asm 
	pop af 
	pop de 
	push de
	push af 

	; get VDP base port
	ld a, (#0x0007)
	inc a
	ld c, a

	; set VRAM address
	out	(c), e
	set 6, d
	out	(c), d

	ret
__endasm;	
}

void vdp_send_value(u_char value, u_char count) __naked {
__asm 
	pop af 
	pop hl 
	push hl
	push af 

	; get VDP data port
	ld a, (#0x0007)
	ld c, a

vdp_send_value_loop:
	out (c), l
	dec h
	jp nz, vdp_send_value_loop
	ret

__endasm;	
}

void vdp_send_data(void *source, u_char count) __naked {
__asm 
	pop af
	pop hl
	pop bc
	push bc
	push hl
	push af

	ld a, c 
	ld b, a

	; get VDP data port
	ld a, (#0x0007)
	ld c, a

vdp_send_data_loop:
	outi
	jp nz, vdp_send_data_loop
	ret
__endasm;
}

void enableScreen() {
__asm
  call #0x0044
__endasm;
}

void disableScreen() {
__asm
  call #0x0041
__endasm;
}

void clearSprites() {
__asm
  call #0x0069
__endasm;
}

u_int random16() {
__asm
rand16:
	ld BC,(#0xF40B)
	ld HL,#253
	xor A
	sbc HL,BC
	sbc A,B
	sbc HL,BC
	sbc A,B
	ld C,A
	sbc HL,BC
	jr nc,rand16end
	inc HL
rand16end:
	ld (#0xF40B),HL
	ret
__endasm;
	return *(u_int*)(0xF40B);
}
