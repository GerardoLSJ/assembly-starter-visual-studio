.586
.MODEL FLAT, C
.STACK
.DATA
factorial DD 5
.CODE
text_test PROC
		mov eax, factorial
loop1:	dec factorial
		imul eax, factorial
		cmp factorial, 1
		jg loop1
		ret
text_test ENDP

END