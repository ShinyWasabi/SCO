#include "Pointers.hpp"
#include "Natives.hpp"
#include "Loader.hpp"
#include "util/Joaat.hpp"
#include "rage/scrThread.hpp"
#include <MinHook.h>

namespace SCOL
{
	static LRESULT(*WndProcOriginal)(HWND, UINT, WPARAM, LPARAM) = nullptr;
	static LRESULT WndProcDetour(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		if (umsg == WM_KEYUP && wparam == g_Variables.ReloadKey)
		{
			Loader::ReloadAllScripts();
		}

		return WndProcOriginal(hwnd, umsg, wparam, lparam);
	}

	static DWORD Main(PVOID)
	{
		Settings::Load();
		Logger::Log("Settings loaded. ScriptsFolder={}, ReloadKey={}", g_Variables.ScriptsFolder, g_Variables.ReloadKey);

		if (!g_Pointers.Init())
		{
			MessageBoxA(0, "Some patterns could not be found.", "SCOL", MB_ICONERROR);
			return EXIT_FAILURE;
		}

		MH_Initialize();
		MH_CreateHook(g_Pointers.WndProc, WndProcDetour, reinterpret_cast<void**>(&WndProcOriginal));
		MH_EnableHook(g_Pointers.WndProc);
		Logger::Log("Enabled hook for WndProc.");

		Logger::Log("Waiting for global blocks to load.");
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