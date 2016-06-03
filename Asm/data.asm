data 4 data
load $2 #0
store $2 *0
load $3 #24
store $3 *2
load $1 *2
load $4 #24
= $1 $4 $5
je L1
jmp L0
label L1:
load $6 #5
store $6 *0
load $7 #2
load $0 *0
* $7 $0 $8
load $9 #1
+ $8 $9 $10
store $10 *0
load $11 #2
load $0 *0
* $11 $0 $12
load $13 #1
+ $12 $13 $14
store $14 *0
load $15 #2
load $0 *0
* $15 $0 $16
load $17 #1
+ $16 $17 $18
store $18 *0
load $19 #2
load $0 *0
* $19 $0 $20
load $21 #1
+ $20 $21 $22
store $22 *0
load $23 #6
store $23 *2
load $24 #2
load $1 *2
* $24 $1 $25
load $26 #1
+ $25 $26 $27
store $27 *2
load $28 #2
load $1 *2
* $28 $1 $29
load $30 #1
+ $29 $30 $31
store $31 *2
load $32 #2
load $1 *2
* $32 $1 $33
load $34 #1
+ $33 $34 $35
store $35 *2
label L0:
halt
#