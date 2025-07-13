#include "Hooking.hpp"
#include "Engine.hpp"
#include <thread>
#include <unordered_map>

using RegisterNativeCommand = void (*)(PVOID table, rage::scrNativeHash hash, rage::scrNativeHandler handler);
using CreateScriptThread    = std::uint32_t(*)(const char* path, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
using StartNewGtaThread     = std::uint32_t(*)(std::uint32_t hash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);

static PVOID nativeRegistrationTable                  = nullptr;
static RegisterNativeCommand registerNativeCommand    = nullptr;
static CreateScriptThread createScriptThread          = nullptr;
static StartNewGtaThread startNewGtaThread            = nullptr;
static PVOID createScriptThreadCaller                 = nullptr; // This is different than the one we're calling
static rage::atArray<rage::scrThread*>* scriptThreads = nullptr;

static CallHook createScriptThreadHook = {};
static const char* currentScriptPath   = nullptr;
static std::vector<std::uint32_t> scriptThreadIds;
static std::unordered_map<std::string, std::optional<Memory>> cachedResults;

std::uint32_t CreateScriptThreadHook(std::uint64_t hash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize)
{
	return createScriptThread(currentScriptPath, args, argCount, stackSize);
}

void NativeCommandWriteMemory(rage::scrNativeCallContext* ctx)
{
	auto name    = ctx->GetArg<const char*>(0);
	auto pattern = ctx->GetArg<const char*>(1);
	auto offset  = ctx->GetArg<int>(2);
	auto rip     = ctx->GetArg<int>(3);
	auto patch   = ctx->GetArg<std::uint64_t*>(4);
	auto protect = ctx->GetArg<int>(5);

	std::optional<Memory> addr = std::nullopt;
	if (cachedResults.contains(name))
	{
		addr = cachedResults[name];
	}
	else
	{
		addr = Memory::ScanPattern(pattern);
		cachedResults[name] = addr;
	}

	if (addr)
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

void NativeCommandReadMemory(rage::scrNativeCallContext* ctx)
{
	auto name    = ctx->GetArg<const char*>(0);
	auto pattern = ctx->GetArg<const char*>(1);
	auto offset  = ctx->GetArg<int>(2);
	auto rip     = ctx->GetArg<int>(3);

	std::optional<Memory> addr = std::nullopt;
	if (cachedResults.contains(name))
	{
		addr = cachedResults[name];
	}
	else
	{
		addr = Memory::ScanPattern(pattern);
		cachedResults[name] = addr;
	}

	int retVal = 0;
	if (addr)
	{
		auto loc = addr->Add(offset);
		if (rip)
			loc = loc.Rip();

		retVal = loc.As<std::int32_t&>();
	}

	ctx->SetReturnValue<int>(static_cast<int>(retVal));
}

void RegisterCustomNatives()
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
	registerNativeCommand(nativeRegistrationTable, 0xEEE74A05DE4C2A07, NativeCommandWriteMemory);
	registerNativeCommand(nativeRegistrationTable, 0x1E9F7F45D0E77AAC, NativeCommandReadMemory);
}

void LoadAllScripts()
{
	WIN32_FIND_DATAA findData;
	auto hFind = FindFirstFileA("SCO\\*.SCO", &findData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		auto fullPath     = std::string("SCO\\") + findData.cFileName;
		currentScriptPath = fullPath.c_str();

		if (!createScriptThreadHook)
		{
			createScriptThreadHook = CallSiteHook::AddHook(createScriptThreadCaller, reinterpret_cast<PVOID>(CreateScriptThreadHook));
		}
		createScriptThreadHook->Enable();

		// We don't need to create a script program as createScriptThread will handle it for us
		if (auto id = startNewGtaThread(0x0000, nullptr, 0, 5050)) // TO-DO: Add custom arg and stack size support
		{
			scriptThreadIds.push_back(id);
		}

		if (createScriptThreadHook)
		{
			createScriptThreadHook->Disable();
		}

		currentScriptPath = nullptr;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
}

void ReloadAllScripts()
{
	for (auto id : scriptThreadIds)
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

	if (auto addr = Memory::ScanPattern("56 57 48 83 EC 28 48 89 CF E8 ? ? ? ? 89 C6 89 C1"))
	{
		startNewGtaThread = addr->As<StartNewGtaThread>();
	}

	if (auto addr = Memory::ScanPattern("E8 ? ? ? ? 89 C6 89 C1 E8 ? ? ? ? 48 85 C0"))
	{
		createScriptThreadCaller = addr->As<PVOID>();
	}

	if (auto addr = Memory::ScanPattern("48 8B 05 ? ? ? ? 48 89 34 F8 48 FF C7 48 39 FB 75 97"))
	{
		scriptThreads = addr->Add(3).Rip().As<rage::atArray<rage::scrThread*>*>();
	}

	if (!nativeRegistrationTable || !registerNativeCommand || !createScriptThread || !startNewGtaThread || !createScriptThreadCaller || !scriptThreads)
		return EXIT_FAILURE;

	// Wait until startup loads
	while (!rage::FindScriptThread(scriptThreads, 0x41D6F794))
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