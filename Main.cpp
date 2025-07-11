#include "Memory.hpp"
#include "Engine.hpp"
#include <thread>

using RegisterNativeCommand = void (*)(PVOID table, rage::scrNativeHash hash, rage::scrNativeHandler handler);
using CreateScriptThread    = std::uint32_t(*)(const char* path, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);

static PVOID nativeRegistrationTable                  = nullptr;
static RegisterNativeCommand registerNativeCommand    = nullptr;
static CreateScriptThread createScriptThread          = nullptr;
static rage::atArray<rage::scrThread*>* scriptThreads = nullptr;

static std::vector<std::uint32_t> scriptThreadIds;

void NativeCommandPatchMemory(rage::scrNativeCallContext* ctx)
{
	auto pattern = ctx->GetArg<const char*>(0);
	auto offset  = ctx->GetArg<int>(1);
	auto value   = ctx->GetArg<int*>(2);
	auto size    = ctx->GetArg<int>(3);

	if (auto addr = Memory::ScanPattern(pattern))
	{
		auto loc = addr->Add(offset);

		DWORD oldProtect;
		VirtualProtect(loc.As<void*>(), size, PAGE_EXECUTE_READWRITE, &oldProtect);

		for (int i = 0; i < size; i++)
		{
			*loc.Add(i).As<std::uint8_t*>() = value[i];
		}

		VirtualProtect(loc.As<void*>(), size, oldProtect, &oldProtect);
	}
}

void RegisterCustomNatives()
{
	/*
		The native hash can be anything as long as it's a full 64-bit number
		I use the joaat hash of the native's name for the lower 32 bits, and fill the rest randomly

		Example usage of this native (model spawn bypass):
		NATIVE PROC PATCH_MEMORY(STRING pattern, INT offset, INT& patch[], INT size) = "0x59C783B684653AC8"	
		INT patch[1]
		patch[0] = 235 // 0xEB
		PATCH_MEMORY("48 8B 06 48 89 F1 FF 50 ? 84 C0 75 ? 31 F6 48 89 F0", 11, patch, 1)
	*/
	registerNativeCommand(nativeRegistrationTable, 0x59C783B684653AC8, NativeCommandPatchMemory);
}

void LoadAllScripts()
{
	WIN32_FIND_DATAA findData;
	auto hFind = FindFirstFileA("SCO\\*.SCO", &findData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		auto fullPath = std::string("SCO\\") + findData.cFileName;

		// We don't need to create a script program as createScriptThread will handle it for us
		auto id = createScriptThread(fullPath.c_str(), nullptr, 0, 5050); // TO-DO: Add custom arg and stack size support
		scriptThreadIds.push_back(id);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
}

void ReloadAllScripts()
{
	for (std::uint32_t id : scriptThreadIds)
	{
		if (auto thread = rage::FindScriptThreadById(scriptThreads, id))
		{
			thread->Kill();
		}
	}

	scriptThreadIds.clear();
	LoadAllScripts();
}

DWORD Main(PVOID)
{
	if (auto addr = Memory::ScanPattern("4C 8D 0D ? ? ? ? 4C 8D 15 ? ? ? ? 45 31 F6"))
	{
		nativeRegistrationTable = addr->Add(3).Rip().As<PVOID>();
	}

	if (auto addr = Memory::ScanPattern("4A 8B 34 E9 48 85 F6 74 16"))
	{
		registerNativeCommand = addr->Sub(0x1C).As<RegisterNativeCommand>();
	}

	if (auto addr = Memory::ScanPattern("48 81 EC 38 02 00 00 45 89 CE"))
	{
		createScriptThread = addr->Sub(0xC).As<CreateScriptThread>();
	}

	if (auto addr = Memory::ScanPattern("48 8B 05 ? ? ? ? 48 89 34 F8 48 FF C7 48 39 FB 75 97"))
	{
		scriptThreads = addr->Add(3).Rip().As<rage::atArray<rage::scrThread*>*>();
	}

	if (!nativeRegistrationTable || !registerNativeCommand || !createScriptThread || !scriptThreads)
		goto exit;

	// Wait until main_persistent loads
	while (!rage::FindScriptThread(scriptThreads, 0x5700179C))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	RegisterCustomNatives();
	LoadAllScripts();

	while (true)
	{
		if (GetAsyncKeyState(VK_F5) & 1)
		{
			ReloadAllScripts();
		}

		std::this_thread::yield();
	}

exit:
	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, PVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(dllInstance);
		CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
	}

	return TRUE;
}