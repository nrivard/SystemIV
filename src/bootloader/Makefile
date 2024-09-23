NAME=bootloader
EXT=mbr
BINARY=$(NAME).$(EXT)
MAPFILE=$(NAME).map
DISASSEMBLY=$(NAME).dis
ELF=$(NAME).a

CPU?=68000

AS=vasmm68k_mot
CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy
OBJDUMP=m68k-elf-objdump

ASFLAGS=-Felf -quiet -align -esc
CFLAGS=-I ./include -I ./cstdlib/include -march=$(CPU) -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-exceptions -fno-delete-null-pointer-checks -Wall
LDFLAGS=-T linker.ld -Map=$(MAPFILE)

SOURCES=mbr.s
# Assume each source files makes an object file
OBJECTS=$(addsuffix .o,$(basename $(SOURCES)))

%o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

all: $(BINARY)
	$(MAKE) bootloader

$(BINARY): $(ELF) $(DISASSEMBLY)
	@$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJECTS)
	@$(LD) $(LDFLAGS) -nostdlib $^ -o $(ELF)

bootloader: $(BINARY)
	@echo "Creating bootloader..."
	@truncate -s 446 $(BINARY)
	@echo "Done."
	@echo "To install, run 'sudo ./mbr -f $(BINARY) DISK' where 'DISK' is your SD card, like '/dev/disk14'"

$(DISASSEMBLY): $(ELF)
	$(OBJDUMP) --disassemble -S $< > $@

clean:
	@rm $(OBJECTS) $(BINARY) $(BINARY_EVEN) $(BINARY_ODD) $(ELF) $(DISASSEMBLY)


