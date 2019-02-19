.data 12
.stack 1000
.code
proc play:
	sub $sp 0
	load $0 &0
	load $7 1
	- $0 $7 $8
	store $8 &0;a
	load $1 &2
	load $9 1
	- $1 $9 $10
	store $10 &2;b
	load $2 &4
	load $11 1
	- $2 $11 $12
	store $12 &4;c
endp
proc draw:
	;x @2
	;y @4
	;a1 @0
	;b1 @2
	;c1 @4
	;d1 @6
	sub $sp 8
	load $14 @0
	store $14 @0;a1
	load $15 @2
	store $15 @2;b1
	load $16 @0
	store $16 @4;c1
	load $17 @2
	store $17 @6;d1
	load $18 @4
	store $18 @0;e1
	load $19 @6
	store $19 @0;f1
	push $bp
	load $bp $sp
	sub $sp 2
	call play
	add $sp 2
	load $sp $bp
	pop $bp
endp
proc main:
	;a @0
	sub $sp 2
	load $26 10
	load $27 11
	push $bp
	load $bp $sp
	push $27
	push $26
	sub $sp 2
	call draw
	add $sp 2
	pop $26
	pop $27
	load $sp $bp
	pop $bp
endp
#