#pragma once
#include "rage/scrNativeCallContext.hpp"

namespace rage
{
	template<typename T>
	class atArray;
	class scrThread;
}
class GtaThread;
union scrValue;

namespace SCOL
{
	namespace Functions
	{
		using RegisterNativeCommand = void (*)(PVOID table, rage::scrNativeHash hash, rage::scrNativeHandler handler);
		using LoadAndStartScriptObj = std::uint32_t(*)(const char* path, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
		using RegisterScriptHandler = std::uint32_t(*)(PVOID _this, GtaThread* thread);
		using KillGtaThread = void(*)(GtaThread* thread);
		using sysVirtualFree = bool(*)(void* ptr);
	}

	struct PointerData
	{
		PVOID WndProc;
		PVOID NativeRegistrationTable;
		Functions::RegisterNativeCommand RegisterNativeCommand;
		Functions::LoadAndStartScriptObj LoadAndStartScriptObj;
		PVOID ScriptHandlerMgrPtr;
		Functions::RegisterScriptHandler RegisterScriptHandler;
		Functions::KillGtaThread KillGtaThread;
		rage::atArray<rage::scrThread*>* ScriptThreads;
		PVOID AllocateGlobalBlock;
		scrValue** ScriptGlobals;
		std::uint32_t* LoadingScreenState;
		Functions::sysVirtualFree sysVirtualFree;
	};

	struct Pointers : PointerData
	{
		bool Init();
	};

	inline Pointers g_Pointers;
}