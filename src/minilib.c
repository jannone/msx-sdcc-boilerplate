#include "minilib.h"

#pragma disable_warning 85

#define public 

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

void _dummy() __naked {
__asm
	VDP_DATA_PORT .equ 0x0007
	BIOS_DISSCR .equ 0x0041
	BIOS_ENASCR .equ 0x0044
	BIOS_WRTVDP .equ 0x0047
	BIOS_FILVRM .equ 0x0056
	BIOS_LDIRVM .equ 0x005C
	BIOS_CHGCLR .equ 0x0062
	BIOS_CLRSPR .equ 0x0069
	BIOS_GTSTCK .equ 0x00D5
	BIOS_GTTRIG .equ 0x00D8
	BIOS_GICINI .equ 0x0090
__endasm;
}

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

public void init() {
  relocate_callbios_from_rom_to_ram();
}

public void set_mode(u_int mode) __naked {
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

public u_char get_stick(u_char id) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	push hl
	push de
	push bc
	push ix

	ld a, 4(ix)
	ld ix, #BIOS_GTSTCK
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

public bool get_trigger(u_char id) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	push ix
	ld a, 4(ix)

	ld ix, #BIOS_GTTRIG
	call callbios
	pop ix
	ld l, a

	pop ix           ;epilogue
	ret

__endasm;
}

public void psg_init() {
__asm
	push ix
	ld ix, #BIOS_GICINI
	call callbios
	pop ix
__endasm;
}

public u_char get_vdp(u_char reg) {
	return *(u_char *)(0xF3DF + reg);
}

public void set_vdp(u_char reg, u_char value) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	push bc

	ld c, 4(ix)
	ld b, 5(ix)

	ld ix, #BIOS_WRTVDP
	call callbios

	pop bc

	pop ix           ;epilogue
	ret

__endasm;
}

public void vwrite(void *source, u_int dest, u_int count) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
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

	ld ix, #BIOS_LDIRVM
	call callbios

	pop ix
	pop de
	pop bc
	pop hl

	pop ix           ;epilogue
	ret

__endasm;
}

public void vfill(u_int addr, u_char value, u_int count) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	push af
	push bc
	push hl

	ld l, 4(ix)
	ld h, 5(ix)
	ld c, 7(ix)
	ld b, 8(ix)
	ld a, 6(ix)

	ld ix, #BIOS_FILVRM
	call callbios

	pop bc
	pop hl
	pop af

	pop ix           ;epilogue
	ret
                            
__endasm;
}

public void set_color(u_char front, u_char back, u_char border) {
	*(u_char*)0xf3e9 = front;
	*(u_char*)0xf3ea = back;
	*(u_char*)0xf3eb = border;
__asm
	push ix
	ld ix, #BIOS_CHGCLR
	call callbios
	pop ix	
__endasm;
}

public void set_mangled_mode() {
	set_mode(mode_1);
	set_mode(0x7E);
	vwrite((void *)0x1BBF, 0x0800, 0x800);
	vwrite((void *)0x1BBF, 0x1000, 0x800);
	vfill(8192, 0xF0, 0x17FF);
	vfill(0xFF8, 0xFF, 8);
	vfill(0x17F8, 0xFF, 8);
}

public void set_sprite_mode(u_char mode) {
	u_char m = get_vdp(1);
	set_vdp(1, (m & 0xFC) | mode);
}

public void set_sprite_16(u_char handle, void *data) {
	vwrite_fast(data, 14336 + (handle << 5), 32);
}

public void buffer_sprite_16(sprite_t *sp, const int x, const int y, const u_char handle, const u_char color) {
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

public void put_sprite_16(const u_char id, const int x, const int y, const u_char handle, const u_char color) {
	static sprite_t sp;
  buffer_sprite_16(&sp, x, y, handle, color);
  vwrite_fast(&sp, 6912 + (id << 2), 4);
}

public void vwrite_fast(void *source, u_int dest, u_char count) __naked {
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
	ld a, (#VDP_DATA_PORT)
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

public void vfill_fast(u_int addr, u_char value, u_char count) __naked {
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
	ld a, (#VDP_DATA_PORT)
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

public void vdp_set_address(u_int addr) __naked {
__asm 
	pop af 
	pop de 
	push de
	push af 

	; get VDP base port
	ld a, (#VDP_DATA_PORT)
	inc a
	ld c, a

	; set VRAM address
	out	(c), e
	set 6, d
	out	(c), d

	ret
__endasm;	
}

public void vdp_send_value(u_char value, u_char count) __naked {
__asm 
	pop af 
	pop hl 
	push hl
	push af 

	; get VDP data port
	ld a, (#VDP_DATA_PORT)
	ld c, a

vdp_send_value_loop:
	out (c), l
	dec h
	jp nz, vdp_send_value_loop
	ret

__endasm;	
}

public void vdp_send_data(void *source, u_char count) __naked {
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
	ld a, (#VDP_DATA_PORT)
	ld c, a

vdp_send_data_loop:
	outi
	jp nz, vdp_send_data_loop
	ret
__endasm;
}

public void bios_enable_screen() {
__asm
  call #BIOS_ENASCR
__endasm;
}

public void bios_disable_screen() {
__asm
  call #BIOS_DISSCR
__endasm;
}

public void bios_clear_sprites() {
__asm
  call #BIOS_CLRSPR
__endasm;
}

public u_int get_random16() {
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
