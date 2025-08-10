#pragma once
#include "rage/scrNativeCallContext.hpp"

namespace rage
{
	template<typename T>
	class atArray;
	class scrThread;
}
class GtaThread;

namespace SCOL
{
	namespace Functions
	{
		using RegisterNativeCommand = void (*)(PVOID table, rage::scrNativeHash hash, rage::scrNativeHandler handler);
		using CreateScriptThread = std::uint32_t(*)(const char* path, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
		using RegisterScriptHandler = std::uint32_t(*)(PVOID _this, GtaThread* thread);
		using KillGtaThread = void(*)(GtaThread* thread);
	}

	struct PointerData
	{
		PVOID WndProc;
		PVOID NativeRegistrationTable;
		Functions::RegisterNativeCommand RegisterNativeCommand;
		Functions::CreateScriptThread CreateScriptThread;
		PVOID ScriptHandlerMgrPtr;
		Functions::RegisterScriptHandler RegisterScriptHandler;
		Functions::KillGtaThread KillGtaThread;
		rage::atArray<rage::scrThread*>* ScriptThreads;
	};

	struct Pointers : PointerData
	{
		bool Init();
	};

	inline Pointers g_Pointers;
}