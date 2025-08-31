#pragma once
#include "rage/scrNativeCallContext.hpp"

namespace rage
{
	template<typename T>
	class atArray;
	class scrThread;
	class scrProgram;
	class scrThreadContext;
	enum scrThreadState : std::uint32_t;
}
class GtaThread;

namespace SCOL
{
	namespace Functions
	{
		using RegisterNativeCommand = void(*)(PVOID table, rage::scrNativeHash hash, rage::scrNativeHandler handler);
		using LoadAndStartScriptObj = std::uint32_t(*)(const char* path, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
		using RegisterScriptHandler = std::uint32_t(*)(PVOID _this, GtaThread* thread);
		using KillGtaThread = void(*)(GtaThread* thread);
		using sysVirtualFree = bool(*)(void* ptr);
		using RunScriptThread = rage::scrThreadState(*)(rage::scrValue* stack, rage::scrValue** globals, rage::scrProgram* program, rage::scrThreadContext* context);
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
		rage::scrValue** ScriptGlobals;
		std::uint32_t* LoadingScreenState;
		Functions::sysVirtualFree sysVirtualFree;
		rage::scrProgram** ScriptPrograms;
		Functions::RunScriptThread RunScriptThread;
	};

	struct Pointers : PointerData
	{
		bool Init();
	};

	inline Pointers g_Pointers;
}