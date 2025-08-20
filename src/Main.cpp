#include "Pointers.hpp"
#include "Hooking.hpp"
#include "Natives.hpp"
#include "Loader.hpp"
#include "rage/scrThread.hpp"

namespace SCOL
{
	static DWORD Main(PVOID)
	{
		Logging::Init("SCOL.log");
		LOGF(INFO, "Logging initialized.");

		Settings::Init("SCOL.ini");
		LOGF(INFO, "Settings initialized. ScriptsFolder={}, ReloadKey={}", g_Variables.ScriptsFolder, g_Variables.ReloadKey);

		if (!g_Pointers.Init())
		{
			MessageBoxA(0, "Some patterns could not be found.", "SCOL", MB_ICONERROR);
			return EXIT_FAILURE;
		}
		LOGF(INFO, "Pointers initialized.");

		if (!Hooking::Init())
		{
			MessageBoxA(0, "Failed to initialize hooking.", "SCOL", MB_ICONERROR);
			return EXIT_FAILURE;
		}
		LOGF(INFO, "Hooking initialized.");

		while (!rage::scrThread::FindScriptThread("Startup"_J))
			std::this_thread::sleep_for(100ms);
		
		Natives::RegisterNatives();
		Loader::LoadScripts();

		while (true)
		{
			Settings::Update();

			std::this_thread::yield();
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