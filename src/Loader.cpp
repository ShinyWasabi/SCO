#include "Loader.hpp"
#include "Pointers.hpp"
#include "ScriptFunction.hpp"
#include "gta/GtaThread.hpp"

namespace SCOL::Loader
{
	static std::vector<std::uint32_t> scriptThreadIds{};

	void LoadScripts()
	{
		auto scriptsFolder = std::filesystem::absolute(g_Variables.ScriptsFolder);

		if (!std::filesystem::exists(scriptsFolder) || !std::filesystem::is_directory(scriptsFolder))
		{
			LOGF(WARNING, "Scripts folder is invalid: {}", scriptsFolder.string());
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(scriptsFolder))
		{
			if (!entry.is_regular_file() || entry.path().extension().string() != ".sco")
				continue;

			auto path = entry.path().string();
			auto name = entry.path().stem().string();
			auto data = Settings::GetScriptData(name);
			LOGF(INFO, "Loaded data for script '{}'. ArgCount={}, StackSize={}, CleanupFunction=0x{:X}", name, data.ArgCount, data.StackSize, data.CleanupFunction);
			if (auto id = g_Pointers.LoadAndStartScriptObj(path.c_str(), data.ArgCount ? data.Args.data() : nullptr, data.ArgCount * sizeof(rage::scrValue), data.StackSize))
			{
				if (auto thread = reinterpret_cast<GtaThread*>(rage::scrThread::FindScriptThreadById(id)))
				{
					g_Pointers.RegisterScriptHandler(g_Pointers.ScriptHandlerMgrPtr, thread);

					scriptThreadIds.push_back(id);
					LOGF(INFO, "Started new thread with ID {}.", id);
				}
			}
		}
	}

	void ReloadScripts()
	{
		if (*g_Pointers.LoadingScreenState != 0)
			return;

		for (auto id : scriptThreadIds)
		{
			if (auto thread = reinterpret_cast<GtaThread*>(rage::scrThread::FindScriptThreadById(id)))
			{
				if (auto data = Settings::GetScriptData(thread->m_ScriptName); data.CleanupFunction != 0)
				{
					ScriptFunction::Call(thread->m_ScriptHash, data.CleanupFunction); // We assume the function doesn't take any arguments. Return type doesn't matter.
				}

				// Even if a script calls TERMINATE_THIS_THREAD (which internally calls scrThread::Kill),
				// it only sets the thread state to KILLED if the thread is the current thread and does
				// not release the script program. We call KillGtaThread here to ensure that the program is
				// released, so that AllocateGlobalBlock is called for the next load, which we need in order to reset globals.
				g_Pointers.KillGtaThread(thread); // thread->Kill();
				LOGF(INFO, "Killed thread with ID {}.", id);
			}
		}

		scriptThreadIds.clear();
		LoadScripts();
	}
}