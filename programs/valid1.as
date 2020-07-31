
l1: .string " s "
    l2: .data 1
 l3: .data -1
    l4: .data 1,2 ,3 , 4, 5  ,   6

;/* TODO saves only 'abc' */
l5: .string "abc"cba"acb"

.data +1,2,+3


 .entry  l1
    .entry  l1

.extern  ext1
.extern     ext1
.extern     ext2

;/* TODO 21 gives: Missing comma at ' '*/
cmp ext1  , ext2
cmp     #-10, ext1
cmp     r1 , l1
cmp r0,    r1

lea l3 ,r0
lea l1 ,l2

;comment!

clr ext1
END: clr r4

jmp ext1
jmp &END

rts
stop