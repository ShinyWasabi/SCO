#include "Pointers.hpp"
#include "Memory.hpp"

namespace SCOL
{
	bool Pointers::Init()
	{
		if (auto addr = Memory::ScanPattern("WndProc", "3D 85 00 00 00 0F 87 2D 02 00 00"))
		{
			WndProc = addr->Sub(0x4F).As<PVOID>();
		}

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

		if (auto addr = Memory::ScanPattern("ScriptHandlerMgrPtr&RegisterScriptHandler", "48 8D 0D ? ? ? ? 48 89 C2 E8 ? ? ? ? 8B 15"))
		{
			ScriptHandlerMgrPtr = addr->Add(3).Rip().As<PVOID>();
			RegisterScriptHandler = addr->Add(0xB).Rip().As<Functions::RegisterScriptHandler>();
		}

		if (auto addr = Memory::ScanPattern("KillGtaThread", "48 89 F2 FF 50 ? 0F BE 86"))
		{
			KillGtaThread = addr->Sub(0x21).As<Functions::KillGtaThread>();
		}

		if (auto addr = Memory::ScanPattern("ScriptThreads", "48 8B 05 ? ? ? ? 48 89 34 F8 48 FF C7 48 39 FB 75 97"))
		{
			ScriptThreads = addr->Add(3).Rip().As<rage::atArray<rage::scrThread*>*>();
		}

		if (!WndProc || !NativeRegistrationTable || !RegisterNativeCommand || !CreateScriptThread || !ScriptHandlerMgrPtr || !RegisterScriptHandler || !KillGtaThread || !ScriptThreads)
			return false;

		return true;
	}
}