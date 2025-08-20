#include "Hooking.hpp"
#include "Pointers.hpp"
#include "Loader.hpp"
#include "rage/scrProgram.hpp"

namespace SCOL
{
    static LRESULT WndProcDetour(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
    {
        if (umsg == WM_KEYUP && wparam == g_Variables.ReloadKey)
        {
            Loader::ReloadScripts();
        }

        return Hooking::GetOriginal<decltype(&WndProcDetour)>("WndProcHook")(hwnd, umsg, wparam, lparam);
    }

    static void AllocateGlobalBlockDetour(rage::scrProgram* program)
    {
        const auto idx = program->GetGlobalBlockIndex();
        if (g_Pointers.ScriptGlobals[idx] != nullptr)
        {
            LOGF(INFO, "Global block {} has already been allocated, freeing it.", idx);

            g_Pointers.sysVirtualFree(g_Pointers.ScriptGlobals[idx]);
            g_Pointers.ScriptGlobals[idx] = nullptr;
        }

        Hooking::GetOriginal<decltype(&AllocateGlobalBlockDetour)>("AllocateGlobalBlockHook")(program);
    }

    bool Hooking::Init()
    {
        if (MH_Initialize() != MH_OK)
            return false;

        AddHook("WndProcHook", g_Pointers.WndProc, WndProcDetour);
        AddHook("AllocateGlobalBlockHook", g_Pointers.AllocateGlobalBlock, AllocateGlobalBlockDetour);

        bool success = true;

        for (auto& hook : m_Hooks)
        {
            if (auto result = MH_CreateHook(hook.m_Target, hook.m_Detour, &hook.m_Original); result != MH_OK)
            {
                LOGF(FATAL, "Failed to create hook for {} ({}).", hook.m_Name, MH_StatusToString(result));
                success = false;
                continue;
            }

            if (auto result = MH_EnableHook(hook.m_Target); result != MH_OK)
            {
                LOGF(FATAL, "Failed to enable hook for {} ({}).", hook.m_Name, MH_StatusToString(result));
                success = false;
            }
        }

        return success;
    }
}