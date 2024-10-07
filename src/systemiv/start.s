    public START

    section .text.init

START:
    or.w    #$0700,SR                   ; Disable interrupts (main can re-enable when ready)
.SetupStacks:                           ; SSP should be top of addr space with return address to ROM added
    lea.l   _os_start,A0                ; set USP to top of user-space (start of OS)
    move.l  A0,USP                      ; growing down (though exec will re-write this)
.Boot:
    jsr     sysmain
.Halt:                                  ; should never get here but if we do spin like hell
    jmp     .Halt
