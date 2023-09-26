CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy

CFLAGS=-I ./src -march=68000 -nostdlib -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall

ASM_FILES=$(shell find ./src -name "*.s")
ASM_OBJECTS=$(patsubst ./src/%, ./obj/%,$(patsubst %.s, %.os, $(ASM_FILES)))

all: 
	@mkdir -p obj
	@$(MAKE) startup

startup: $(ASM_OBJECTS)
	@$(LD) -nostdlib $^ -o startup.o
	@$(OBJCOPY) -O binary startup.o startup
	@truncate -s 4096 startup

obj/%.os: src/%.s
	@echo "    AS $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
