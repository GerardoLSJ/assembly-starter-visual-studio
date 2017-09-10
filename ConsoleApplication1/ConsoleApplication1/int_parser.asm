.586
.MODEL FLAT, C
.STACK
.DATA
string_input DB "hello world",0
.CODE
                            ;EXAMPLE: -2 == 0xFFFF FFFE
int_parser PROC
	lea eax, string_input	;load effective adress of this.string
	mov ebx, 0				;Contador init in zero
	
for_loop:	
	mov cl, [eax + ebx]		;Tomamos solo la "H" porque [CX CH CL] solo tomas el ultimo valor
	cmp cl, 20h
	je space				;Si "cl" vale 20 en HEX es un espacio, no lo incrementes
	sub cl, 20h				;Restamos 20 a la "h"
space:		
	mov [eax + ebx], cl		; Guarda la "H" en la direccion de memoria indicada
	inc ebx					;incrementamos el contadr
	mov ch, [eax + ebx]
	cmp ch,0				;compara si es 0 en esa localidad de memoria
	jne for_loop			;jump not equal
	ret

    mov edx, num3entered ; our string
atoi:
    xor eax, eax ; zero a "result so far"
.top:
    movzx ecx, byte [edx] ; get a character
    inc edx ; ready for next one
    cmp ecx, '0' ; valid?
    jb .done
    cmp ecx, '9'
    ja .done
    sub ecx, '0' ; "convert" character to number
    imul eax, 10 ; multiply "result so far" by ten
    add eax, ecx ; add in current digit
    jmp .top ; until done
.done:
    ret

int_parser ENDP

END