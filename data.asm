data
4
data
load $2 #0
store $2 *0
load $3 #20
store $3 *1
label L0:
load $0 *0
load $1 *1
! $0 $1 $4
jne L2
jmp L1
label L2:
load $0 *0
load $5 #2
+ $0 $5 $6
store $6 *0
jmp L0
label L1:
load $0 *0
load $7 #20
= $0 $7 $8
je L4
jmp L5
label L4:
load $1 *1
load $9 #1
+ $1 $9 $10
store $10 *1
load $1 *1
load $0 *0
+ $1 $0 $11
store $11 *0
jmp L3
label L5:
load $1 *1
load $12 #2
+ $1 $12 $13
store $13 *1
load $1 *1
load $0 *0
+ $1 $0 $14
store $14 *0
label L3:
