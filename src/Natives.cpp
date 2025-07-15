#include "Natives.hpp"
#include "Memory.hpp"
#include "Pointers.hpp"

namespace SCO::Natives
{
	/*
		The native hash can be anything as long as it's a full 64-bit number
		I use the joaat hash of the native's name for the lower 32 bits, and fill the rest randomly

		Example writing/reading memory:
		NATIVE PROC WRITE_MEMORY(STRING name, STRING pattern, INT offset, BOOL rip, INT& patch[], BOOL protect) = "0xEEE74A05DE4C2A07"
		NATIVE FUNC INT READ_MEMORY(STRING name, STRING pattern, INT offset, BOOL rip) = "0x1E9F7F45D0E77AAC"

		INT patch[1]
		patch[0] = 235 // 0xEB
		WRITE_MEMORY("ModelSpawnBypass", "48 8B 06 48 89 F1 FF 50 ? 84 C0 75 ? 31 F6 48 89 F0", 11, FALSE, patch, TRUE)

		INT gameTimer = READ_MEMORY("GameTimer", "3B 2D ? ? ? ? 76 ? 89 D9", 2, TRUE)
	*/

	static constexpr auto WRITE_MEMORY_HASH = 0xEEE74A05DE4C2A07;
	static constexpr auto READ_MEMORY_HASH = 0x1E9F7F45D0E77AAC;

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

	static void RegisterNative(rage::scrNativeHash hash, rage::scrNativeHandler handler)
	{
		g_Pointers.RegisterNativeCommand(g_Pointers.NativeRegistrationTable, hash, handler);
		Logger::Log("Registered native with hash 0x{:X}.", hash);
	}

	void RegisterCustomNatives()
	{
		RegisterNative(WRITE_MEMORY_HASH, NativeCommandWriteMemory);
		RegisterNative(READ_MEMORY_HASH, NativeCommandReadMemory);
	}
}