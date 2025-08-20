#include "Pointers.hpp"
#include "Scanner.hpp"

namespace SCOL
{
    bool Pointers::Init()
    {
        Scanner scanner;

        scanner.Add("WndProc", "3D 85 00 00 00 0F 87 2D 02 00 00", [this](Memory addr) {
            WndProc = addr.Sub(0x4F).As<PVOID>();
        });

        scanner.Add("NativeRegistrationTable", "4C 8D 0D ? ? ? ? 4C 8D 15 ? ? ? ? 45 31 F6", [this](Memory addr) {
            NativeRegistrationTable = addr.Add(3).Rip().As<PVOID>();
        });

        scanner.Add("RegisterNativeCommand", "4A 8B 34 E9 48 85 F6 74 16", [this](Memory addr) {
            RegisterNativeCommand = addr.Sub(0x1C).As<Functions::RegisterNativeCommand>();
        });

        scanner.Add("LoadAndStartScriptObj", "48 81 EC 38 02 00 00 45 89 CE", [this](Memory addr) {
            LoadAndStartScriptObj = addr.Sub(0xC).As<Functions::LoadAndStartScriptObj>();
        });

        scanner.Add("ScriptHandlerMgrPtr&RegisterScriptHandler", "48 8D 0D ? ? ? ? 48 89 C2 E8 ? ? ? ? 8B 15", [this](Memory addr) {
            ScriptHandlerMgrPtr = addr.Add(3).Rip().As<PVOID>();
            RegisterScriptHandler = addr.Add(0xB).Rip().As<Functions::RegisterScriptHandler>();
        });

        scanner.Add("KillGtaThread", "48 89 F2 FF 50 ? 0F BE 86", [this](Memory addr) {
            KillGtaThread = addr.Sub(0x21).As<Functions::KillGtaThread>();
        });

        scanner.Add("ScriptThreads", "48 8B 05 ? ? ? ? 48 89 34 F8 48 FF C7 48 39 FB 75 97", [this](Memory addr) {
            ScriptThreads = addr.Add(3).Rip().As<rage::atArray<rage::scrThread*>*>();
        });

        scanner.Add("AllocateGlobalBlock", "E8 ? ? ? ? 4C 89 E1 E8 ? ? ? ? 41 BE FF 3F", [this](Memory addr) {
            AllocateGlobalBlock = addr.Add(1).Rip().As<PVOID>();
        });

        scanner.Add("ScriptGlobals", "48 8B 8E B8 00 00 00 48 8D 15 ? ? ? ? 49 89 D8", [this](Memory addr) {
            ScriptGlobals = addr.Add(7).Add(3).Rip().As<scrValue**>();
        });

        scanner.Add("LoadingScreenState", "8B 0D ? ? ? ? 83 C1 F7", [this](Memory addr) {
            LoadingScreenState = addr.Add(2).Rip().As<std::uint32_t*>();
        });

        return scanner.Scan();
    }
}