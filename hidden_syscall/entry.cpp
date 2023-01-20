#include <windows.h>
#include <winternl.h>
#include "hidden_syscall.hpp"

__declspec(noinline) NTSTATUS nt_close
(
	HANDLE handle
)
{
	return hidden_syscall::call
	(
		0xf,
		handle
	);
}

int entry()
{
	hidden_syscall::init();

	nt_close(0);
	
	return 0;
}