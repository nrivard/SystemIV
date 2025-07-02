rwildcard=$(wildcard $1) $(foreach d,$1,$(call rwildcard,$(addsuffix /$(notdir $d),$(wildcard $(dir $d)*))))

NAME=systemiv
EXT=bin
BINARY=$(NAME).$(EXT)
MAPFILE=$(NAME).map
DISASSEMBLY=$(NAME).dis
ELF=kernel.a

CPU?=68000

AS=vasmm68k_mot
CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy
OBJDUMP=m68k-elf-objdump

ASFLAGS=-Felf -quiet -align -esc -I ./hardware/include
CFLAGS=-I ./include -I ./cstdlib/include -I ./hardware/include -I ./binary_data -march=$(CPU) -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-exceptions -fno-delete-null-pointer-checks -Wall -O1
LDFLAGS=-T linker.ld -Map=$(MAPFILE)
# DEFINES=-D SPIGATE

# Assume source files in Makefile directory are source files for project
CSOURCES=$(call rwildcard,*.c)
ASMSOURCES=$(call rwildcard,*.s)
INCLUDES=$(call rwildcard,*.h)
SOURCES=$(ASMSOURCES) $(CSOURCES)

# Assume each source files makes an object file
OBJECTS=$(addsuffix .o,$(basename $(SOURCES)))

%.o : %.c
	$(CC) -c $(CFLAGS) $(DEFINES) -o $@ $<

%.o: %.s 
	$(AS) $(ASFLAGS) -o $@ $<

all: $(BINARY) $(INCLUDES)
	@$(MAKE) kernel

$(BINARY): $(ELF) $(DISASSEMBLY)
	@$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJECTS)
	@$(LD) $(LDFLAGS) -nostdlib $^ -o $(ELF)

kernel: $(BINARY_EVEN) $(BINARY_ODD)
	@echo "Creating kernel..."

$(DISASSEMBLY): $(ELF)
	$(OBJDUMP) --disassemble -S $< > $@

clean:
	@rm $(OBJECTS) $(BINARY) $(BINARY_EVEN) $(BINARY_ODD) $(ELF) $(DISASSEMBLY)
