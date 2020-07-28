;Label redefinition
CODELABEL: .data 5
CODELABEL: mov r0,r1

;Label redefinition
CODELABEL2: mov r0,r1
CODELABEL2: .data 5

;Label redefinition to external
CODELABEL3: mov r0,r1
.extern CODELABEL3


; command doesnt exist
01234567890123456789012345678901234567890123456789012345678901234567890123456789

; line too long
012345678901234567890123456789012345678901234567890123456789012345678901234567891

; no error
0123456789012345678901234567890: .data 1

;label too long /* TODO */
01234567890123456789012345678901: .data 1

; illegal comma /* TODO */
.data ,1

; extra text /* TODO */
.data 1,

; consecutive commas /* TODO */
.data 1,,2

;not a number /* TODO */
.data 1a2

;not a number
.data 1-2

;not a number
.data 12a /* TODO */

; no quotes
.string sadfsa

; no end quote
.string "sdafsdf /* TODO */

; no start quote
.string sdafsdf"


;no arguments
.data

;no string
.string

; no argument
.entry

; no argument
.external

;too many arguments /* TODO */
.data 1 extra text

;too many arguments /* TODO */
.string "asdfsad" extra text

;too many arguments
entrylabel: .data 5
.entry entrylabel extra text

;too many arguments
.extern externalLabel extra text

; Operand count errors /* TODO 82 */
mov
mov r0
cmp #-0,MYLABEL,HERE
add
add #-4,#-9,#7
sub LABEL
lea operand
clr
not
inc r-9,r1,r4
dec
jmp
bne &realtivelyjump,notToday
jsr
red alpha,beta
prn
rts hello
stop myprog

; Addressing type errors (by booklet p.34)


mov &op0, op1
mov &op0, &op1
mov op0, &op1
mov &op0, #2
mov op0, #2
mov #-4,#2

cmp &op0, op1
cmp &op0, r0
cmp r0, &op1

add &op0, #2
add &op0, op1
add #-4, &op1
add r0, &op1
add #-4,#2

sub &op0, #2
sub &op0, op1
sub &op0, &op1
sub r0, &op1
sub op0, #2

lea #-4, #2
lea #-4, r0
lea r0, op1
lea r0, &op1
lea &op0, r0

clr &op0
not &op0
inc #-1
dec &op0
red &op0
; /* TODO 37, 38 */
jmp op0
bne op0
jsr #-1

prn &op0