.data 12
.stack 1000
.code
proc draw:
	;x @2
	;y @4
	;a1 @0
	;b1 @2
	;c1 @4
	;d1 @6
	;e1 @8
	;f1 @10
	- $sp 12
	load $7 @2
	store $7 @0;a1
	load $8 @4
	store $8 @2;b1
	load $9 @0
	store $9 @4;c1
	load $10 @2
	store $10 @6;d1
	load $11 @4
	store $11 @8;e1
	load $12 @6
	store $12 @10;f1
endp
proc main:
	;a @0
	- $sp 2
	load $18 10
	load $19 11
	push $bp
	load $bp $sp
	push $19
	push $18
	- $sp 2
	call draw
	+ $sp 2
	pop $18
	pop $19
	load $sp $bp
	pop $bp
endp
