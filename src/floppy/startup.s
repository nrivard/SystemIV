; 68k Mac boot floppy example
;
; Boot block header based on first/first.S
; from http://emile.sourceforge.net/
;
; More resources:
;     http://www.mac.linux-m68k.org/devel/macalmanac.php
;     http://www.pagetable.com/?p=50
;     Linux and BSD kernel source, e.g.
;         https://github.com/torvalds/linux/tree/master/arch/m68k/mac

; The ROM loads this many bytes from the start of the disk.
stage1_size = 1024

; Macro to define a Pascal string
Str15 macro
        dc.b    \?1-2, \1               ; arg count includes both '"' chars for some reason
        cnop    15-(\?1-2),1            ; so correct both incorrect counts by subtracting them
    endm

section text

; begin:

ID:          dc.w   $4C4B              ; HFS boot blocks signature */
Entry:       bra.w  _start             ; entry point to bootcode */
Version:     dc.w   $4418              ; boot blocks version number */
PageFlags:   dc.w   $0000              ; used internally */
SysName:     Str15  "KernelSanders"    ; System filename */
ShellName:   Str15  "0123456789ABCEF"  ; Finder filename */
Dbg1Name:    Str15  ""   ; debugger filename */
Dbg2Name:    Str15  "foo bar"   ; debugger filename */
ScreenName:  Str15  "foo bar"   ; name of startup screen */
HelloName:   Str15  "foo bar"   ; name of startup program */
ScrapName:   Str15  "foo bar"   ; name of system scrap file */
CntFCBs:     dc.w   10                 ; number of FCBs to allocate */
CntEvts:     dc.w   20                 ; number of event queue elements */
Heap128K:    dc.l   $00004300          ; system heap size on 128K Mac */
Heap256K:    dc.l   $00008000          ; used internally */
SysHeapSize: dc.l   $00020000          ; system heap size on all machines */

_start::
    ; Now running your code with full privs, what more do you want? ;)
    move.l  #$BEEF,d0  ; Error code for the Sad Mac screen
    dc.w    $A9C9      ; Call the SysError trap

padding:
    cnop 0,stage1_size
