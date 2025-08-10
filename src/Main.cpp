#include "Pointers.hpp"
#include "Hooking.hpp"
#include "Natives.hpp"
#include "Loader.hpp"
#include "rage/scrThread.hpp"

namespace SCOL
{
	static DWORD Main(PVOID)
	{
		Settings::Load();
		Logger::Log("Settings loaded. ScriptsFolder={}, ReloadKey={}", g_Variables.ScriptsFolder, g_Variables.ReloadKey);

		if (!g_Pointers.Init())
		{
			MessageBoxA(0, "Some patterns could not be found.", "SCOL", MB_ICONERROR);
			return EXIT_FAILURE;
		}

		if (!Hooking::Init())
		{
			MessageBoxA(0, "Failed to initialize hooking.", "SCOL", MB_ICONERROR);
			return EXIT_FAILURE;
		}
		Logger::Log("Hooking initialized.");

		while (!rage::scrThread::FindScriptThread("Startup"_J)) // Use landing_pre_startup instead?
			std::this_thread::sleep_for(100ms);

		Natives::RegisterNatives();
		Loader::LoadAllScripts();

		while (true)
		{
			Settings::Update();

			std::this_thread::sleep_for(100ms);
		}

		return EXIT_SUCCESS;
	}
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, PVOID)
{
	using namespace SCOL;

	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(dllInstance);
		CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
	}

	return TRUE;
}