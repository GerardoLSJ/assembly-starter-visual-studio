.586
.MODEL FLAT, C
.STACK
.DATA
;el numero ingresado termina en 0h PLOX
string_input DB "-123",0h
len equ $ - string_input 	;reescribir
result DD 00000000,0h
temp DD 1,0h
success DD 1,0h
negativo DD 0,0h
.CODE
;ebx : contadpr
                            ;EXAMPLE: -2 == 0xFFFF FFFE
int_parser PROC
	mov ecx, len 
	mov eax,1
	;lea eax, string_input	;load effective adress of this.string
	;vamos a usar un registro tempral para leer strin_input

	lea edx, result			;load effective adress of result
	mov ebx, len-2			;Contador
	;mov ecx, [eax + ebx]	;Para ver si es signo	
	;call checkSign			;Checar el signo

for_loop:
	lea edi, string_input
	mov cl, [edi + ebx]		;Tomamos el primer char porque [CX, CH, CL] 16 bits, 8 bits, 8 bits 

	cmp  cl, '0' 			; Validate is a number in this intercal 
	jb lessZero				; oterwise finish JA, JB when comparing unsigned integers,
	cmp  cl, '9'
	ja greaterNine

	sub cl, '0'				;ELSE is a number
	;mov [edx + ebx], cl		; Guarda el "NUMERO" en la direccion de memoria indicada
	mov esi, ecx
	;imul esi,[temp]
	imul esi, eax			; multiplicar ecx por 1,10,100,1000

	add result, esi

	xor esi,esi
	mov esi, eax			;mover 1,10 a esi para multiplicar
	imul esi,10				;
	;lea edi, temp
	mov eax, esi 			; mover el 10,100,1000 a EAX
	dec ebx					; counter ++
	
	lea edi, string_input
	mov esi, [edi + ebx - 1]		;Vemos si sigue un 0
	cmp esi, 0				;compara si es 0 en esa localidad de memoria
	jne for_loop			;jump not equal
	ret

lessZero:
	xor ecx,ecx		;end 
	; terminamos, vamos a checar si hubo signo
	call checkSign
	ret
	
greaterNine:
	xor ecx,ecx		;end
	; terminamos, vamos a checar si hubo signo
	call checkSign
	ret

checkSign:
	lea edi, string_input
	mov cl, [edi]
	cmp  cl, 2bh
	je positiveNumber
	cmp  cl,2dh
	je negativeNumber
	ret

positiveNumber:
	; encender bandera de que es un numero positivo
	; no hacemos nada
	
	ret
jmp done
negativeNumber:
	; negamos la respuesta
	neg result
	jmp done
	ret

done:
ret
int_parser ENDP

END