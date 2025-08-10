#include "Hooking.hpp"
#include "Pointers.hpp"
#include "Loader.hpp"
#include "rage/scrProgram.hpp"
#include <MinHook.h>

namespace SCOL::Hooking
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

	static void(*AllocateGlobalBlockOriginal)(rage::scrProgram*) = nullptr;
	static void AllocateGlobalBlockDetour(rage::scrProgram* program)
	{
		if (g_Pointers.ScriptGlobals[program->GetGlobalBlockIndex()] != nullptr) // It should be R* handling this, not us... Currently globals are only freed when all scripts are terminated, which only happens when returning to the main menu or exiting the game.
		{
			Logger::Log("Global block {} has already been allocated.", program->GetGlobalBlockIndex());
			return; // We can free the block using game's VirtualFree, but that would also wipe existing global values, which is not really good.
		}

		AllocateGlobalBlockOriginal(program);
	}

	bool Init()
	{
		if (MH_Initialize() != MH_OK)
			return false;

		MH_CreateHook(g_Pointers.WndProc, WndProcDetour, reinterpret_cast<void**>(&WndProcOriginal));
		MH_CreateHook(g_Pointers.AllocateGlobalBlock, AllocateGlobalBlockDetour, reinterpret_cast<void**>(&AllocateGlobalBlockOriginal));

		return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
	}
}