EXTERN ntdll_stub: QWORD

.code

syscall_stub proc
	mov r10, rcx
	mov eax, [rsp + 28h] 
	pop r12 
	lea r11, [ret_address]
	mov [rsp], r11
	mov r11, [ntdll_stub]
	jmp r11
	ret
ret_address:
	sub rsp, 10h
	mov [rsp], r12
	ret
syscall_stub endp

end