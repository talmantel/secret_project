; line too long
012345678901234567890123456789012345678901234567890123456789012345678901234567891

;label too long
01234567890123456789012345678901: .data 1

;Label redefinition
CODELABEL: .data 5
CODELABEL: mov r0,r1

;Label redefinition
CODELABEL2: mov r0,r1
CODELABEL2: .data 5

;Label redefinition to external
CODELABEL3: mov r0,r1
.extern CODELABEL3

;Label redefinition from external
.extern CODELABEL4
CODELABEL4: mov r0,r1

;Label redefinition from external
.extern CODELABEL5
CODELABEL5: .data 5

;Label redefinition to external
CODELABEL6: .data 5
.extern CODELABEL6

; Empty label
:  .data 1
:  mov r0,r1

; label must begin with a letter
0label:  .data 1
012:  .data 1
0:  .data 1

; label can only contain alphanumeric characters
some,label: mov r0,r1
;-----------------------------------------------------------------------------------------------

; illegal comma
.data ,1

; consecutive commas
.data 1,,2

;missing comma
.data 1 2

; extra text
.data 1,
.data 1 extra text
.string "asdfsad" extra text
.entry entrylabel extra text
.extern externalLabel extra text

;not a number
.data 1a2
.data 1-2
.data 12a
.data 1,-1,a,3
.data 1,-1,1a,3
.data 1,-1,a2,3


; no quotes
.string sadfsa

; no end quote
.string "sdafsdf

; no start quote /* works rename */
.string sdafsdf"

;illegal character in string
.string "	 tab:	"


;missing argument
.data
.string
.entry
.extern
;-----------------------------------------------------------------------------------------------

; command doesnt exist
01234567890123456789012345678901234567890123456789012345678901234567890123456789
some label: mov r0,r1
somelabel:mov r0,r1
com r0
com
jmpr,
label: command: r0

; missing operands (needs 2)
mov
mov r0
cmp
cmp r0
add
add r0
sub
sub LABEL
lea
lea operand

; too many operands (needs 2)
mov r0,r1,r2
cmp #-0,LABEL,LABEL2
add #-14,label,#91
sub LABEL,r0,r2
lea label,r1, r2
mov r0,r1,r2,e3
cmp #-0,LABEL,LABEL2,r3
add #-4,label,#5,r2
sub LABEL,r0,r2,r2
lea operand,r4,r5

; missing operands (needs 1)
clr
not
inc
dec
jmp
bne
jsr
red
prn

; too many operands (needs 1)
clr r1,r2
not r1,r2
inc r1,r2
dec r1,r2
jmp r1,r2
bne r1,r2
jsr r1,r2
red r1,r2
prn r1,r2
clr r1,r2 , r3
not r1,r2 , r3
inc r1,r2 , r3
dec r1,r2 , r3
jmp r1,r2 , r3
bne r1,r2 , r3
jsr r1,r2 , r3
red r1,r2 , r3
prn r1,r2 , r3


; too many operands (needs 0)
rts hello
stop myprog
rts hello,hel2
stop myprog,hel2


; Addressing type errors (by booklet p.34)
mov &lb1,lb2
mov &lb1,&lb2
mov lb1,&lb2
mov &lb1,#2
mov lb1,#2
mov #14,#2
cmp &lb1,lb2
cmp &lb1,&r0
cmp r0,&lb2
add &lb1,lb2
add &lb1,&lb2
add lb1,&lb2
add &lb1,#2
add lb1,#2
add #-4,#2
sub &lb1,lb2
sub &lb1,&lb2
sub lb1,&lb2
sub &lb1,#20
sub lb1,#20
sub #10,#20
lea &lb1,lb2
lea &lb1,&lb2
lea lb1,&lb2
lea &lb1,#2
lea lb1,#2
lea #-4,#2
lea #-4,r0
lea r0,lb2
lea r0,r1
clr &lb1
not &lb1
inc &lb1
dec &lb1
red &lb1
clr #9
not #9
inc #9
dec #9
red #9
jmp r0
bne r0
jsr r0
jmp #-9
bne #-9
jsr #-9
prn &lb1
mov &r0, label

;label without instruction
labelwithoutinstruct:
          labelwithoutinstruct:

;label is using reserved words
mov:
cmp:
add:
sub:
lea:
clr:
not:
inc:
dec:
jmp:
bne:
jsr:
red:
prn:
rts:
stop:
r0:
r1:
r2:
r3:
r4:
r5:
r6:
r7:

;label in command is a reserved word
jmp mov
jmp cmp
jmp add
jmp sub
jmp lea
jmp clr
jmp not
jmp inc
jmp dec
jmp jmp
jmp bne
jmp jsr
jmp red
jmp prn
jmp rts
jmp stop
jmp &mov
jmp &cmp
jmp &add
jmp &sub
jmp &lea
jmp &clr
jmp &not
jmp &inc
jmp &dec
jmp &jmp
jmp &bne
jmp &jsr
jmp &red
jmp &prn
jmp &rts
jmp &stop
jmp &r0
jmp &r1
jmp &r2
jmp &r3
jmp &r4
jmp &r5
jmp &r6
jmp &r7
lea mov , label
lea cmp , label
lea add , label
lea sub , label
lea lea , label
lea clr , label
lea not , label
lea inc , label
lea dec , label
lea jmp , label
lea bne , label
lea jsr , label
lea red , label
lea prn , label
lea rts , label
lea stop, label
lea label,mov
lea label,cmp
lea label,add
lea label,sub
lea label,lea
lea label,clr
lea label,not
lea label,inc
lea label,dec
lea label,jmp
lea label,bne
lea label,jsr
lea label,red
lea label,prn
lea label,rts
lea label,stop
.extern mov
.extern cmp
.extern add
.extern sub
.extern lea
.extern clr
.extern not
.extern inc
.extern dec
.extern jmp
.extern bne
.extern jsr
.extern red
.extern prn
.extern rts
.extern stop
.extern r0
.extern r1
.extern r2
.extern r3
.extern r4
.extern r5
.extern r6
.extern r7
.entry mov
.entry cmp
.entry add
.entry sub
.entry lea
.entry clr
.entry not
.entry inc
.entry dec
.entry jmp
.entry bne
.entry jsr
.entry red
.entry prn
.entry rts
.entry stop
.entry r0
.entry r1
.entry r2
.entry r3
.entry r4
.entry r5
.entry r6
.entry r7


;label must begin with a letter
mov 1d , label
cmp 1d , label
add 1d , label
sub 1d , label
lea 1d , label
mov label, 1d
cmp label, 1d
add label, 1d
sub label, 1d
lea label, 1d
clr 1s
not 1s
inc 1s
dec 1s
jmp 1s
bne 1s
jsr 1s
red 1s
prn 1s
.entry 1s
.extern 1s
.entry &r0
.extern &r1
jmp .label
mov somelabel, .label
mov .label, label
.entry .label
.extern .label

;illegal character in label in command
jmp labe&l
mov somelabel, labe&l
mov somel&abel, label
.entry labe&l
.extern labe&l


;illegal character in label in command (or wrong addressing)
.entry #label
.extern #label
.entry &label
.extern &label

;label in command too long
jmp l1234567890123456789012345678901
.entry l1234567890123456789012345678901
.extern l1234567890123456789012345678901

;consecutive commas
mov labell, , label2
mov labell,, label2
mov labell ,,label2

;extra text
mov label, label2 extra text
mov label, label2 ,extra text
mov label, label2, extra text
mov label, label2 , extra text
mov label, label2 ,, extra text
mov label, label2 , , extra text