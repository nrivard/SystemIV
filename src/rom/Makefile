NAME=SystemN8
EXT=rom
BINARY=$(NAME).$(EXT)
BINARY_EVEN=$(NAME).even.$(EXT)
BINARY_ODD=$(NAME).odd.$(EXT)
MAPFILE=$(NAME).map

AS=vasmm68k_mot
CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy

ASFLAGS=-Felf -quiet
CFLAGS=-I ./src -march=68000 -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall
LDFLAGS=-T linker.ld -Map=$(MAPFILE)

OBJECTS=startup.o mfp.o

all: 
	@mkdir -p obj
	@$(MAKE) roms

$(BINARY): $(OBJECTS)
	@$(LD) $(LDFLAGS) -nostdlib $^ -o kernel.a
	@$(OBJCOPY) -O binary kernel.a $(BINARY)

%.o: %.s 
	@echo "Assembling...$<"
	@$(AS) $(ASFLAGS) -o $@ $<

roms: $(BINARY_EVEN) $(BINARY_ODD)
	@echo "Creating roms..."

$(BINARY_EVEN): $(BINARY)
	srec_cat -output $(BINARY_EVEN) -Binary $(BINARY) -Binary -Split 2 0

$(BINARY_ODD): $(BINARY)
	srec_cat -output $(BINARY_ODD) -Binary $(BINARY) -Binary -Split 2 1

clean:
	@rm $(OBJECTS) $(BINARY) $(BINARY_EVEN) $(BINARY_ODD)
