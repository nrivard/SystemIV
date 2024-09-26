    public START

    section .text.init

START:
    or.w    #$0700,SR                   ; Disable interrupts
.SetupStacks:

.Done:
    and.w   #$F8FF,SR                   ; enable interrupts
    jsr     sysmain