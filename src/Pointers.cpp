#include "Pointers.hpp"
#include "Memory.hpp"

namespace SCOL
{
	bool Pointers::Init()
	{
		if (auto addr = Memory::ScanPattern("NativeRegistrationTable", "4C 8D 0D ? ? ? ? 4C 8D 15 ? ? ? ? 45 31 F6"))
		{
			NativeRegistrationTable = addr->Add(3).Rip().As<PVOID>();
		}

		if (auto addr = Memory::ScanPattern("RegisterNativeCommand", "4A 8B 34 E9 48 85 F6 74 16"))
		{
			RegisterNativeCommand = addr->Sub(0x1C).As<Functions::RegisterNativeCommand>();
		}

		if (auto addr = Memory::ScanPattern("CreateScriptThread", "48 81 EC 38 02 00 00 45 89 CE"))
		{
			CreateScriptThread = addr->Sub(0xC).As<Functions::CreateScriptThread>();
		}

		if (auto addr = Memory::ScanPattern("StartNewGtaThread", "56 57 48 83 EC 28 48 89 CF E8 ? ? ? ? 89 C6 89 C1"))
		{
			StartNewGtaThread = addr->As<Functions::StartNewGtaThread>();
		}

		if (auto addr = Memory::ScanPattern("CreateScriptThreadCaller", "E8 ? ? ? ? 89 C6 89 C1 E8 ? ? ? ? 48 85 C0"))
		{
			CreateScriptThreadCaller = addr->As<PVOID>();
		}

		if (auto addr = Memory::ScanPattern("ScriptThreads", "48 8B 05 ? ? ? ? 48 89 34 F8 48 FF C7 48 39 FB 75 97"))
		{
			ScriptThreads = addr->Add(3).Rip().As<rage::atArray<rage::scrThread*>*>();
		}

		if (!NativeRegistrationTable || !RegisterNativeCommand || !CreateScriptThread || !StartNewGtaThread || !CreateScriptThreadCaller || !ScriptThreads)
			return false;

		return true;
	}
}