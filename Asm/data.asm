data
		$a [2]
		$b [2]
		$c [2]
		$d [2]
		$e [2]
		$f [2]
		$num [1]
data
func draw:
	push bp
	mov bp sp
	sub sp 6
	sub sp 2
	load $8 x
	load $11 10
	< $8 $11 $12
	jb L1
	jmp L0
L1:
	load $0 a
	load $13 10
	- $0 $13 $14
	store $14 a
L0:
	ret
endf
func main:
	push bp
	mov bp sp
	sub sp 4
	sub sp 2
	load $19 10
	store $19 a
L2:
	load $16 a
	load $20 0
	> $16 $20 $21
	jg L4
	jmp L3
L4:
	load $17 b
	load $24 28
	+ $17 $24 $25
	store $25 b
	load $17 b
	load $26 30
	= $17 $26 $27
	je L8
	jmp L7
L8:
	jmp L5
L7:
	load $17 b
	load $28 1
	- $17 $28 $29
	store $29 b
L5:
	load $16 a
	load $22 1
	- $16 $22 $23
	store $23 a
	jmp L2
L3:
	push bp
	mov bp sp
	call draw
	ret
endf