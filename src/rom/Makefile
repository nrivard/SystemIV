rwildcard=$(wildcard $1) $(foreach d,$1,$(call rwildcard,$(addsuffix /$(notdir $d),$(wildcard $(dir $d)*))))

NAME=SystemN8
EXT=rom
BINARY=$(NAME).$(EXT)
BINARY_EVEN=$(NAME).even.$(EXT)
BINARY_ODD=$(NAME).odd.$(EXT)
MAPFILE=$(NAME).map

CPU?=68000

AS=vasmm68k_mot
CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy

ASFLAGS=-Felf -quiet -align -esc
CFLAGS=-I ./include -I ./cstdlib/include -march=$(CPU) -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-exceptions -fno-delete-null-pointer-checks -Wall
LDFLAGS=-T linker.ld -Map=$(MAPFILE)

# Assume source files in Makefile directory are source files for project
CSOURCES=$(call rwildcard,*.c)
ASMSOURCES=$(call rwildcard,*.s)
INCLUDES=$(call rwildcard,*.h)
SOURCES=$(ASMSOURCES) $(CSOURCES)

# Assume each source files makes an object file
OBJECTS=$(addsuffix .o,$(basename $(SOURCES)))

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.s 
	$(AS) $(ASFLAGS) -o $@ $<

all: $(BINARY) $(INCLUDES)
	@$(MAKE) roms

$(BINARY): $(OBJECTS)
	@$(LD) $(LDFLAGS) -nostdlib $^ -o bootloader.a
	@$(OBJCOPY) -O binary bootloader.a $(BINARY)

roms: $(BINARY_EVEN) $(BINARY_ODD)
	@echo "Creating roms..."

$(BINARY_EVEN): $(BINARY)
	srec_cat -output $(BINARY_EVEN) -Binary $(BINARY) -Binary -Split 2 0

$(BINARY_ODD): $(BINARY)
	srec_cat -output $(BINARY_ODD) -Binary $(BINARY) -Binary -Split 2 1

clean:
	@rm $(OBJECTS) $(BINARY) $(BINARY_EVEN) $(BINARY_ODD) bootloader.a
