.586
.MODEL FLAT, C
.STACK
.DATA
string_input DB "12345",0h
len equ $ - string_input
result DD 00000000,0h
temp DD 1,0h
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
	dec ebx						;counter ++
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