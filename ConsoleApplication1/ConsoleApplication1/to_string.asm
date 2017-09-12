.586
.MODEL FLAT, C
.STACK
.DATA

input_value DD 2ACB451h             ; 123 Es un numero
string_output DB "0000000000"       ;mil millones
char_pos DB 0

.CODE
to_string PROC
; toma el registro de EAX ECX
        lea ecx string_output       ;load effective address
        add ecx, 10                 ;esta es la longitud
        mov eax, input_value        ;parte alta
        mov ebx, 10                 ;divisor 
for_loop:
        mov edx, 0                  ;parte baja un 0
        div ebx                     ;en otro reg32 EBX movemos el 10
        add edx, 30h                ;parseo 
        mov [ecx], dl               ;mover el EDX solo su EL a string_output
        dec ecx                     ;decrementa ECX
        cmp eax,0                   ;comparo con cero
        jne for_loop
        ret
to_string ENDP
END
; en EDX el residuo(3) y EAX un 12 
; residuo = %10 +30 
; le resto el residuo
; numero = numero -residuo
; numero = numero/10 