.586
.MODEL FLAT, C
.STACK
.DATA
string_input DB "hello world",0
.CODE
to_upper PROC
			lea eax, string_input	;load effective adress of this.string
			mov ebx, 0				;Contador init in zero
	
for_loop:	mov cl, [eax + ebx]		;Tomamos solo la "H" porque [CX CH CL] solo tomas el ultimo valor
			cmp cl, 20h
			je space				;Si "cl" vale 20 en HEX es un espacio, no lo incrementes
			sub cl, 20h				;Restamos 20 a la "h"
space:		mov [eax + ebx], cl		; Guarda la "H" en la direccion de memoria indicada
			inc ebx					;incrementamos el contadr
			mov ch, [eax + ebx]
			cmp ch,0				;compara si es 0 en esa localidad de memoria
			jne for_loop			;jump not equal
			ret

to_upper ENDP

END