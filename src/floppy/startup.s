# 68k Mac boot floppy example
#
# Boot block header based on first/first.S
# from http://emile.sourceforge.net/
#
# More resources:
#     http://www.mac.linux-m68k.org/devel/macalmanac.php
#     http://www.pagetable.com/?p=50
#     Linux and BSD kernel source, e.g.
#         https://github.com/torvalds/linux/tree/master/arch/m68k/mac

# The ROM loads this many bytes from the start of the disk.
.equ stage1_size, 1024

# Macro to define a Pascal string
.macro pString string
pstring_begin_\@:
        .byte   pstring_end_\@ - pstring_string_\@ - 1
pstring_string_\@:
        .string "\string"
pstring_end_\@:
        .fill 16 - (pstring_end_\@ - pstring_begin_\@) , 1, 0
.endm

.section .text
.global _start

begin:

ID:          .short  0x4C4B              /* boot blocks signature */
Entry:       bra     _start               /* entry point to bootcode */
Version:     .short  0x4418              /* boot blocks version number */
PageFlags:   .short  0x00                /* used internally */
SysName:     pString "foo bar       "    /* System filename */
ShellName:   pString "foo bar       "    /* Finder filename */
Dbg1Name:    pString "foo bar       "    /* debugger filename */
Dbg2Name:    pString "foo bar       "    /* debugger filename */
ScreenName:  pString "foo bar       "    /* name of startup screen */
HelloName:   pString "foo bar       "    /* name of startup program */
ScrapName:   pString "foo bar       "    /* name of system scrap file */
CntFCBs:     .short  10                  /* number of FCBs to allocate */
CntEvts:     .short  20                  /* number of event queue elements */
Heap128K:    .long   0x00004300          /* system heap size on 128K Mac */
Heap256K:    .long   0x00008000          /* used internally */
SysHeapSize: .long   0x00020000          /* system heap size on all machines */

_start:
    /* Now running your code with full privs, what more do you want? ;) */

    movel #0xDEAD, %d0  /* Error code for the Sad Mac screen */
    .short 0xA9C9       /* Call the SysError trap */
end:

.fill stage1_size - (end - begin), 1, 0xda
