;Universidad Nacional Autonoma de Mexico
;Facultad de Ingenieria
;Silva Garcia Carlos Sebastian

.586
.MODEL FLAT, C
.STACK
.DATA
string_input DB "-13sd57",0
result DD 0
success DB 00

.CODE
int_parser PROC
	;------------- Iniciamos los registros que utilizaremos ------------------------------
	lea eax, string_input					; asignamos la primera dirección de memoria de string
	mov ebx, 0								; Iniciamos un contador en 0
	mov ecx, 1								; Iniciamos el multimplicador en 1
	mov cl, [eax + ebx]						; Usamos el registro temral cl

	;---Primero checamos si la cadena tiene un signo al inicio-----------
	cmp cl, 2dh								; Comparamos si cl = '-'
	je	starts_with_sign					; Omitimos el signo

	cmp cl, 2bh								; Comparamos si cl = '-'
	je	starts_with_sign					; Omitimos el signo

	starts_with_sign: 
		inc ebx

	iterate_string: 
		mov cl, [eax + ebx]				; Usamos el registro temporal cl
		inc ebx							; Incrementamos el contador en uno

		;----------Validamos para cada digito que sea un numero, si no vamos a error----------
		cmp cl, 30h						; Comparamos cl = 0
		jl error						; cl < 0, entonces NO es numero
		cmp cl, 39h						; comparamos cl con 9
		jg error						; cl > 9, entonces NO es numero

		sub cl, 30h						; Le restamos 30 para obtener el número
		
		;---------Operaciones para multiplicar por unidades, decenas, centenas --------------
		mov edx, result					; Guardamos el resultado anterior en edx
		imul edx, 10					; Multiplicamos por 10 el resultado anterior
		add edx, ecx					; Sumamos el nuevo valor de ecx al resultado anterior ya multiplicado por 10
		mov result, edx					; Movemos el nuevo resultado
		

		mov edx, 1								; edx sera la suma temporal
		mov cl, [eax + ebx]				; Cambiamos al siguiente registro

		cmp cl, 0						; Comparamos ebx con FIN
		jne iterate_string				; Iteramos hasta llegar a 0		
		;ret								; Finalizamos el ciclo

	cmp cl, 0						; Comparamos ebx con FIN
	je iteration_finished			; Una vez que terminó de iterar, jump to iteration_finished		


	iteration_finished: 
		;-----Validaciones--------
		mov success, 1h
		mov cl, [eax]
		cmp cl, 2bh								; Comparamos si cl = "+"
		je is_positive							; vamos a is_positive

		cmp cl, 2dh								; Comparamos si cl = '-'
		je is_negative							; Vamos a is_negative

	is_positive: ret
	is_negative: 
		mov edx, result
		neg edx
		mov result, edx
		ret

	error:
		mov success, 0
		ret

	ret
int_parser ENDP
END