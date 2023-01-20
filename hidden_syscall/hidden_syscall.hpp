#define LAZY_IMPORTER_NO_CPP_FORWARD

#include "lazy_importer.hpp"

#include <cstdlib>
#include <stdint.h>

#ifndef HIDDEN_SYSCALL_HPP
#define HIDDEN_SYSCALL_HPP

namespace hidden_syscall
{
	size_t syscall_exports_count = 0;

	extern "C" void syscall_stub();
	extern "C" uintptr_t ntdll_stub = 0;

	void find_ntdll_stub()
	{
		const auto tick_count = *reinterpret_cast<uint64_t*>(0x7ffe0320);
		const auto preudo_rnd = tick_count + (tick_count * tick_count * 0xDEADC0DE + 0x13371488);
		const auto rnd_export_index = preudo_rnd % syscall_exports_count;

		uintptr_t stub_address = 0;

		const auto ntdll = LI_MODULE("ntdll.dll").get<char*>();
		const ::li::detail::exports_directory exports(ntdll);

		size_t export_count = 0;

		auto export_index = exports.size();

		while (export_index--)
		{
			const auto export_address =
				reinterpret_cast<uintptr_t>(exports.address(export_index));

			if (*reinterpret_cast<uint16_t*>(export_address + 0x12) == 0x50F)
			{
				export_count++;

				if (export_count == rnd_export_index)
				{
					stub_address = export_address + 0x12;
					break;
				}
			}
		}

		InterlockedExchange64(reinterpret_cast<LONG64*>(&ntdll_stub), stub_address);
	}

	void init()
	{
		const auto ntdll = LI_MODULE("ntdll.dll").get<char*>();
		const ::li::detail::exports_directory exports(ntdll);

		auto export_index = exports.size();

		while (export_index--)
		{
			const auto export_address =
				reinterpret_cast<uintptr_t>(exports.address(export_index));

			if (*reinterpret_cast<uint16_t*>(export_address + 0x12) == 0x50F)
			{
				syscall_exports_count++;
			}
		}

		find_ntdll_stub();
	}

	template <typename ret_t,
		typename arg1_t = void*,
		typename arg2_t = void*,
		typename arg3_t = void*,
		typename arg4_t = void*,
		typename ...args_t>
	ret_t _syscall(const uint32_t index,
		const arg1_t arg1 = nullptr,
		const arg2_t arg2 = nullptr,
		const arg3_t arg3 = nullptr,
		const arg4_t arg4 = nullptr,
		const args_t ...args)
	{
		return (reinterpret_cast<ret_t(__stdcall*)(arg1_t, arg2_t, arg3_t, arg4_t,
			uint32_t, args_t...)>(&syscall_stub))(arg1, arg2, arg3, arg4, index, args...);
	}

	template <typename ret_t = long, typename ...args_t>
	ret_t call(const uint32_t index, const args_t ...args)
	{
		const auto ret = _syscall<ret_t>(index, args...);
		find_ntdll_stub();
		return ret;
	}
}

#endif