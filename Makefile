SDCC?=sdcc
SDAS?=sdasz80
MAKEBIN?=makebin

UNIX_DD?=dd
UNIX_SED?=sed
UNIX_AWK?=gawk
UNIX_GREP?=grep

ROM16KBLOCKS?=2
ROM16KSKIP?=1

CRTOBJ?=sdcc/crt/crt0msx.32k.4000.rel
CRTSRC?=sdcc/crt/crt0msx.32k.4000.s

CFLAGS?=--code-loc 0x4020 --data-loc 0xc000 -mz80 --no-std-crt0 --fsigned-char

SRCDIR=src
DISTDIR=dist

SRCFILES=$(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(DISTDIR)/%.rel,$(SRCFILES))

all: $(DISTDIR) $(DISTDIR)/program.rom
	rm -f $(DISTDIR)/*.asm $(DISTDIR)/*.lst $(DISTDIR)/*.sym $(DISTDIR)/*.ihx $(DISTDIR)/*.lk $(DISTDIR)/*.map $(DISTDIR)/*.noi

$(DISTDIR): 
	mkdir -p $@

$(DISTDIR)/%.rom: $(DISTDIR)/%.ddmem
	$(UNIX_DD) if=$< of=$@ bs=16384 skip=$(ROM16KSKIP) count=$(ROM16KBLOCKS)

$(DISTDIR)/%.ddmem: $(DISTDIR)/%.ihx
	$(MAKEBIN) -s 65536 < $< > $@

$(DISTDIR)/%.noi: $(DISTDIR)/%.ihx

$(DISTDIR)/%.sdccsym: $(DISTDIR)/%.noi
	$(UNIX_AWK) 'BEGIN { FS=" "; } /DEF.*/ { print $$2 ": equ 0" substr($$3,3) "h"; }' < $< > $<tmp
	-$(UNIX_GREP) -v "_end: equ " $<tmp > $@

$(DISTDIR)/program.ihx: $(OBJFILES) $(CRTOBJ)
	$(SDCC) $(CFLAGS) $(LDFLAGS) -o$@ $(OBJFILES) $(CRTOBJ)

$(DISTDIR)/heap.rel: $(DISTDIR)/heap.s
	$(SDAS) -o heap.rel heap.s

$(DISTDIR)/heap.s: $(SDCC_HOME)/lib/src/z80/heap.s
	$(UNIX_SED) -e 's/1023/9000/' "$(SDCC_HOME)"/lib/src/z80/heap.s > $(DISTDIR)/heap.s

$(DISTDIR)/%.rel: $(SRCDIR)/%.c
	$(SDCC) -c $(CFLAGS) $< -o $@

$(CRTOBJ): $(CRTSRC)
	$(SDAS) -o $(CRTOBJ) $(CRTSRC)

clean:
	$(RM) dist/*

version:
	$(SDCC) -v

.PRECIOUS: %.rel %.ddmem %.ihx %.noi %.rst
