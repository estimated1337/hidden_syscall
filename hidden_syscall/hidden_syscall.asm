EXTERN ntdll_stub: QWORD

.code

syscall_stub proc
	mov r10, rcx
	mov eax, [rsp + 20h + 8] 
	pop r12 
	db 4ch ; lea r11, [rip + 0xf]
	db 8dh
	db 1dh
	db 0fh
	db 00h
	db 00h
	db 00h
	mov [rsp], r11
	mov r11, [ntdll_stub]
	jmp r11
	ret
	sub rsp, 10h
	mov [rsp], r12
	ret
syscall_stub endp

end