#include "Loader.hpp"
#include "Hooking.hpp"
#include "Pointers.hpp"
#include "rage/scrThread.hpp"

namespace SCO::Loader
{
	static CallHook createScriptThreadHook{};
	static std::string currentScriptPath{};
	static std::vector<std::uint32_t> scriptThreadIds;

	static std::uint32_t CreateScriptThreadHook(std::uint64_t hash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize)
	{
		Logger::Log("Loading script from path {}.", currentScriptPath);
		return g_Pointers.CreateScriptThread(currentScriptPath.c_str(), args, argCount, stackSize);
	}

	void LoadAllScripts()
	{
		WIN32_FIND_DATAA findData;
		auto hFind = FindFirstFileA("SCO\\*.SCO", &findData);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		do
		{
			currentScriptPath = std::string("SCO\\") + findData.cFileName;

			if (!createScriptThreadHook)
			{
				createScriptThreadHook = CallSiteHook::AddHook(g_Pointers.CreateScriptThreadCaller, reinterpret_cast<PVOID>(CreateScriptThreadHook));
			}
			createScriptThreadHook->Enable();

			// We don't need to create a script program as createScriptThread will handle it for us
			if (auto id = g_Pointers.StartNewGtaThread(0x0000, nullptr, 0, 5050)) // TO-DO: Add custom arg and stack size support
			{
				Logger::Log("Started new thread with ID {}.", id);
				scriptThreadIds.push_back(id);
			}

			if (createScriptThreadHook)
			{
				createScriptThreadHook->Disable();
			}

			currentScriptPath.clear();

			std::this_thread::sleep_for(100ms);
		} while (FindNextFileA(hFind, &findData));

		FindClose(hFind);
	}

	void ReloadAllScripts()
	{
		for (auto id : scriptThreadIds)
		{
			if (auto thread = rage::scrThread::FindScriptThreadById(id))
			{
				thread->Kill();
				Logger::Log("Killed thread with ID {}.", id);
			}
		}

		scriptThreadIds.clear();
		LoadAllScripts();
	}
}