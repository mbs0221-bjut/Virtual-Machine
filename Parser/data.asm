data 4 data
load $2 #0
store $2 *0
load $3 #24
store $3 *2
load $4 #10
store $4 *0
label L0:
load $0 *0
load $5 #0
> $0 $5 $6
jg L2
jmp L1
label L2:
load $0 *0
load $7 #1
- $0 $7 $8
store $8 *0
load $1 *2
load $9 #1
+ $1 $9 $10
store $10 *2
jmp L0
label L1:
label L3:
load $0 *0
load $11 #10
< $0 $11 $12
jb L5
jmp L4
label L5:
load $0 *0
load $13 #1
+ $0 $13 $14
store $14 *0
jmp L3
label L4:
label L6:
load $1 *2
load $15 #1
- $1 $15 $16
store $16 *2
load $1 *2
load $17 #12
> $1 $17 $18
jg L6
jmp L7
label L7:
load $1 *2
load $0 *0
! $1 $0 $19
jne L9
jmp L8
label L9:
load $20 #64
store $20 *0
load $21 #32
store $21 *2
label L8:
halt
