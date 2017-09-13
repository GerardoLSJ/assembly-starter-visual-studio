; Luis Gerardo Lopez Santibañez Jimenez


.586
.MODEL FLAT, C
.STACK
.DATA
;el numero ingresado termina en 0h PLOX
string_input DB "+10",0h
len equ $ - string_input 	;reescribir
result DD 00000000,0h
success DD 1,0h
.CODE
;ebx : contadpr
                            	;EXAMPLE: -2 == 0xFFFF FFFE
int_parser PROC

lea eax, string_input
mov ebx, 0
xor ecx,ecx
len_string:
	mov cl, [eax + ebx + 1]
	cmp cl, 0
	je end_len
	inc ebx
	jmp len_string

end_len:
	;sub ebx,1
	mov ecx, len 
	mov eax,1					;multiplicador 1, 10, 1000
	;vamos a usar un registro tempral para leer strin_input
	lea edx, result			;load effective adress of result
	;mov ebx, len-2			;Contador

for_loop:
	lea edi, string_input
	mov cl, [edi + ebx]			;Tomamos el primer char porque [CX, CH, CL] 16 bits, 8 bits, 8 bits 

	cmp  cl, '0' 				; Validate is a number in this intercal 
	jb lessZero					; oterwise finish JA, JB when comparing unsigned integers,
	cmp  cl, '9'
	ja greaterNine

	sub cl, '0'					;ELSE is a number
	mov esi, ecx
	imul esi, eax				; multiplicar ecx por 1,10,100,1000

	add result, esi

	xor esi,esi
	mov esi, eax				;mover 1,10 a esi para multiplicar
	imul esi,10				
	mov eax, esi 				; mover el 10,100,1000 a EAX
	dec ebx						; counter ++
	
	lea edi, string_input
	mov esi, [edi + ebx - 1]	;Vemos si sigue un 0
	cmp esi, 0					;compara si es 0 en esa localidad de memoria
	jne for_loop				;jump not equal
	;BREAKPOINT H E R E
	ret

lessZero:
	xor ecx,ecx					;end 
	sub ecx, 1
	cmp ebx,ecx
	je done
	cmp ebx, 0					;Checamos si nuestro contador esta en 0 si no no deberias 
	jne errorchar
	; terminamos, vamos a checar si hubo signo
	call checkSign
	ret
	
greaterNine:
	xor ecx,ecx					;end
	sub ecx, 1
	cmp ebx,ecx
	je done
	cmp ebx, 0					;Checamos si nuestro contador esta en 0 si no no deberias 
	jne errorchar
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
	;BREAKPOINT H E R E	
	ret

positiveNumber:
	; no hacemos nada
	;BREAKPOINT H E R E
	ret
	jmp done
negativeNumber:
	; negamos la respuesta
	neg result

	;BREAKPOINT H E R E
	jmp done
	ret

errorchar:
	mov success,0
	;BREAKPOINT H E R E
	ret
	

done:
;BREAKPOINT H E R E
	ret
int_parser ENDP

END