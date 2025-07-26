#include "Natives.hpp"
#include "Memory.hpp"
#include "Pointers.hpp"
#include "rage/tlsContext.hpp"
#include "rage/scrThread.hpp"

namespace SCOL::Natives
{
	static constexpr rage::scrNativeHash WRITE_MEMORY = 0xEEE74A05DE4C2A07;
	static constexpr rage::scrNativeHash READ_MEMORY = 0x1E9F7F45D0E77AAC;
	static constexpr rage::scrNativeHash SET_CURRENT_SCRIPT_THREAD = 0x7AFACDB81809E2C1;
	static constexpr rage::scrNativeHash WRITE_SCRIPT_STATIC = 0xBB3346E0994B7EA5;
	static constexpr rage::scrNativeHash READ_SCRIPT_STATIC = 0x8725A6C8DE144DBC;

	static void NativeCommandWriteMemory(rage::scrNativeCallContext* ctx)
	{
		auto name = ctx->GetArg<const char*>(0);
		auto pattern = ctx->GetArg<const char*>(1);
		auto offset = ctx->GetArg<int>(2);
		auto rip = ctx->GetArg<int>(3);
		auto patch = ctx->GetArg<std::uint64_t*>(4);
		auto protect = ctx->GetArg<int>(5);

		if (auto addr = Memory::ScanPattern(name, pattern))
		{
			auto loc = addr->Add(offset);
			if (rip)
				loc = loc.Rip();

			auto ptr = loc.As<std::uint8_t*>();

			auto count = static_cast<int>(patch[0]);
			auto items = patch + 1;

			std::vector<std::uint8_t> data;
			for (int i = 0; i < count; i++)
			{
				data.push_back(static_cast<std::uint8_t>(items[i]));
			}

			if (protect)
			{
				DWORD oldProtect, temp;
				VirtualProtect(ptr, count, PAGE_EXECUTE_READWRITE, &oldProtect);
				std::memcpy(ptr, data.data(), count);
				VirtualProtect(ptr, count, oldProtect, &temp);
			}
			else
			{
				std::memcpy(ptr, data.data(), count);
			}
		}
	}

	static void NativeCommandReadMemory(rage::scrNativeCallContext* ctx)
	{
		auto name = ctx->GetArg<const char*>(0);
		auto pattern = ctx->GetArg<const char*>(1);
		auto offset = ctx->GetArg<int>(2);
		auto rip = ctx->GetArg<int>(3);

		int retVal = 0;
		if (auto addr = Memory::ScanPattern(name, pattern))
		{
			auto loc = addr->Add(offset);
			if (rip)
				loc = loc.Rip();

			retVal = loc.As<std::int32_t&>();
		}

		ctx->SetReturnValue<int>(static_cast<int>(retVal));
	}

	static void NativeCommandSetCurrentScriptThread(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->GetArg<joaat_t>(0);

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			rage::tlsContext::Get()->m_CurrentScriptThread = thread;
			rage::tlsContext::Get()->m_ScriptThreadActive = true; // What if it was false?
		}
	}

	static void NativeCommandWriteScriptStatic(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->GetArg<joaat_t>(0);
		auto index = ctx->GetArg<int>(1);
		auto value = ctx->GetArg<int>(2);

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			auto addr = reinterpret_cast<uintptr_t*>((uintptr_t)thread->m_Stack + (index * sizeof(uintptr_t)));
			if (addr)
			{
				*addr = value;
			}
		}
	}

	static void NativeCommandReadScriptStatic(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->GetArg<joaat_t>(0);
		auto index = ctx->GetArg<int>(1);

		int retVal = 0;
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			auto addr = reinterpret_cast<uintptr_t*>((uintptr_t)thread->m_Stack + (index * sizeof(uintptr_t)));
			if (addr)
			{
				retVal = *reinterpret_cast<int*>(addr);
			}
		}

		ctx->SetReturnValue<int>(static_cast<int>(retVal));
	}

	void RegisterNatives()
	{
		static auto RegisterNative = [](rage::scrNativeHash hash, rage::scrNativeHandler handler) {
			g_Pointers.RegisterNativeCommand(g_Pointers.NativeRegistrationTable, hash, handler);
			Logger::Log("Registered native with hash 0x{:X}.", hash);
		};

		RegisterNative(WRITE_MEMORY, NativeCommandWriteMemory);
		RegisterNative(READ_MEMORY, NativeCommandReadMemory);
		RegisterNative(SET_CURRENT_SCRIPT_THREAD, NativeCommandSetCurrentScriptThread);
		RegisterNative(WRITE_SCRIPT_STATIC, NativeCommandWriteScriptStatic);
		RegisterNative(READ_SCRIPT_STATIC, NativeCommandReadScriptStatic);
	}
}