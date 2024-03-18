NAME=SystemN8
EXT=rom
BINARY=$(NAME).$(EXT)
BINARY_EVEN=$(NAME).even.$(EXT)
BINARY_ODD=$(NAME).odd.$(EXT)

AS=vasmm68k_mot
CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy

ASFLAGS=-Felf
CFLAGS=-I ./src -march=68000 -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall

ASM_FILES=$(shell find ./ -name "*.s")
ASM_OBJECTS=$(patsubst ./%, ./obj/%,$(patsubst %.s, %.os, $(ASM_FILES)))

all: 
	@mkdir -p obj
	@$(MAKE) roms

$(BINARY): $(ASM_OBJECTS)
	@$(LD) -nostdlib $^ -o startup.o
	@$(OBJCOPY) -O binary startup.o $(BINARY)
	@truncate -s 1M $(BINARY)

$(ASM_OBJECTS): $(ASM_FILES)
	@echo "    AS $<"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -o $@ $<

roms: $(BINARY_EVEN) $(BINARY_ODD)
	@echo "    Creating roms"

$(BINARY_EVEN): $(BINARY)
	srec_cat -output $(BINARY_EVEN) -Binary $(BINARY) -Binary -Split 2 0

$(BINARY_ODD): $(BINARY)
	srec_cat -output $(BINARY_ODD) -Binary $(BINARY) -Binary -Split 2 1
