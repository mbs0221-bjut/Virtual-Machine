data
		$a [2]
		$b [2]
		$c [2]
		$d [2]
		$e [2]
		$f [2]
		$num [1]
data
proc draw:
	push bp
	mov bp sp
	sub sp 6
	sub sp 2
	load $8 #sp + 2;x
	load $11 10
	< $8 $11 $12
	jb L1
	jmp L0
L1:
	load $0 #ds + 2;a
	load $13 10
	- $0 $13 $14
	store $14 a
L0:
	ret
endp
proc play:
	push bp
	mov bp sp
	sub sp 6
	sub sp 2
	load $16 #sp + 2;x
	load $17 #sp + 4;y
	+ $16 $17 $20
	load $18 #sp + 6;z
	+ $20 $18 $21
	store $21 p
	ret
endp
proc main:
	push bp
	mov bp sp
	sub sp 4
	sub sp 2
	load $26 10
	store $26 a
L2:
	load $23 #sp + 2;a
	load $27 0
	> $23 $27 $28
	jg L4
	jmp L3
L4:
	load $24 #sp + 4;b
	load $31 28
	+ $24 $31 $32
	store $32 b
	load $24 #sp + 4;b
	load $33 30
	= $24 $33 $34
	je L8
	jmp L7
L8:
	jmp L5
L7:
	load $24 #sp + 4;b
	load $35 1
	- $24 $35 $36
	store $36 b
L5:
	load $23 #sp + 2;a
	load $29 1
	- $23 $29 $30
	store $30 a
	jmp L2
L3:
	push bp
	mov bp sp
	load $38 10
	load $39 11
	load $40 12
	push $40
	push $39
	push $38
	call draw
	pop $38
	pop $39
	pop $40
	push bp
	mov bp sp
	load $42 10
	load $43 11
	load $44 12
	push $44
	push $43
	push $42
	call play
	pop $42
	pop $43
	pop $44
	ret
endp
