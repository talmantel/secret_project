;first pass tests that should pass
r8: .data 1
r01: .data 1
bne r8
bne r01



;definitions for second pass testing
label: .data 1
doesntexist:  .extern ex
doesntexist: .entry label
doesntexist:


;----------------------------------------------------------------

;label doesnt exist
jmp doesntexist
jmp &doesntexist
mov doesntexist, label
mov label, doesntexist
.entry doesntexist

;extern as relative operand
jmp &ex

;extern entry
.entry ex
