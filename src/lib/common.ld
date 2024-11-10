OUTPUT_FORMAT("elf32-m68k")

/* physical memory layout */
MEMORY
{
  	RAM	: org = 0x00000000, l = 1M
  	ROM	: org = 0x00400000, l = 1M
	XPN	: org = 0x00800000, l = 1M
  	MFP	: org = 0x00E00000, l = 1M
}

PROVIDE(_bootloader_start = 0x00002000); /* addr to load bootloader */
PROVIDE(_os_start = 0x00080000);         /* addr to load OS */
PROVIDE(_phys_end = ORIGIN(RAM) + LENGTH(RAM));
