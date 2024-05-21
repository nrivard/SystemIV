AS=vasmm68k_mot
CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy

ASFLAGS=-Felf
CFLAGS=-I ./src -march=68000 -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall -O2

ASM_FILES=$(shell find ./src -name "*.s")
ASM_OBJECTS=$(patsubst ./src/%, ./obj/%,$(patsubst %.s, %.os, $(ASM_FILES)))

all: 
	@mkdir -p obj
	@$(MAKE) bootloader

bootloader: $(ASM_OBJECTS)
	@$(LD) -nostdlib $^ -o startup.o
	@$(OBJCOPY) -O binary startup.o SystemN8.dsk
	@truncate -s 4096 SystemN8.dsk

obj/%.os: src/%.s
	@echo "    AS $<"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -o $@ $<
