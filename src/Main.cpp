#include "Pointers.hpp"
#include "Natives.hpp"
#include "Loader.hpp"
#include "util/Joaat.hpp"
#include "rage/scrThread.hpp"

namespace SCO
{
	static DWORD Main(PVOID)
	{
		Settings::Load();
		Logger::Log("Settings loaded. ScriptsFolder={} ReloadKey={}", g_Variables.ScriptsFolder, g_Variables.ReloadKey);

		if (!g_Pointers.Init())
		{
			MessageBoxA(0, "Some patterns could not be found.", "SCO", MB_ICONERROR);
			return EXIT_FAILURE;
		}

		Logger::Log("Waiting for global blocks to load.");
		while (!rage::scrThread::FindScriptThread("Startup"_J)) // Use landing_pre_startup instead?
			std::this_thread::sleep_for(100ms);

		Natives::RegisterCustomNatives();
		Loader::LoadAllScripts();

		while (true)
		{
			Settings::Update();

			if (GetAsyncKeyState(g_Variables.ReloadKey) & 1)
			{
				Loader::ReloadAllScripts();
			}

			std::this_thread::sleep_for(100ms);
		}

		return EXIT_SUCCESS;
	}
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, PVOID)
{
	using namespace SCO;

	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(dllInstance);
		CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
	}

	return TRUE;
}