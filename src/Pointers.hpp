#pragma once
#include "rage/scrNativeCallContext.hpp"

namespace rage
{
	template<typename T>
	class atArray;
	class scrThread;
}

namespace SCOL
{
	namespace Functions
	{
		using RegisterNativeCommand = void (*)(PVOID table, rage::scrNativeHash hash, rage::scrNativeHandler handler);
		using CreateScriptThread = std::uint32_t(*)(const char* path, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
		using StartNewGtaThread = std::uint32_t(*)(std::uint32_t hash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
	}

	struct PointerData
	{
		PVOID NativeRegistrationTable;
		Functions::RegisterNativeCommand RegisterNativeCommand;
		Functions::CreateScriptThread CreateScriptThread; // This automatically creates a script program
		Functions::StartNewGtaThread StartNewGtaThread;
		PVOID CreateScriptThreadCaller; // This is different than the one we're calling
		rage::atArray<rage::scrThread*>* ScriptThreads;
	};

	struct Pointers : PointerData
	{
		bool Init();
	};

	inline Pointers g_Pointers;
}