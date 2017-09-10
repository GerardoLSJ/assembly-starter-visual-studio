.586
.MODEL FLAT, C
.STACK
.DATA
string_input DB "12345",0h
len equ $ - string_input
result DB 00000000,0h
.CODE
;ebx : contadpr
                            ;EXAMPLE: -2 == 0xFFFF FFFE
int_parser PROC
	mov ecx, len 
	lea eax, string_input	;load effective adress of this.string
	lea edx, result			;load effective adress of result
	mov ebx, len-2			;Contador
	;mov ecx, [eax + ebx]	;Para ver si es signo	
	;call checkSign			;Checar el signo

for_loop:
	mov cl, [eax + ebx]		;Tomamos el primer char porque [CX, CH, CL] 16 bits, 8 bits, 8 bits 

	cmp  cl, '0' 			; Validate is a number in this intercal 
	jb lessZero				; oterwise finish
	cmp  cl, '9'
	ja greaterNine

	sub cl, '0'				;ELSE is a number
	;mov [edx + ebx], cl		; Guarda el "NUMERO" en la direccion de memoria indicada
	movzx  cx , cl
	imul cx, 10
	add result, cx
	dec ebx					; counter ++
	
	mov ch, [eax + ebx - 1]		;Vemos si sigue un 0
	cmp ch, 0				;compara si es 0 en esa localidad de memoria
	jne for_loop			;jump not equal
	ret

lessZero:
	xor ecx,ecx		;end 
	jecxz done 
	
greaterNine:
	xor ecx,ecx		;end
	jecxz done

checkSign:
	cmp  cl, '+'
	je positiveNumber
	cmp  cl, '-'
	je negativeNumber
	ret

positiveNumber:
	; encender bandera de que es un numero positivo
	xor ecx,ecx
	dec ebx					;counter ++
	;mov cl, [eax + ebx]		;Tomamos el siguiete valor porque fue signo
	ret

negativeNumber:
	; encender bandera de que es un numero positivo
	xor ecx,ecx
	dec ebx					;counter ++
	;mov cl, [eax + ebx]		;Tomamos el siguiete valor porque fue signo
	ret

done:
ret
int_parser ENDP

END