#include "Loader.hpp"
#include "Pointers.hpp"
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
			LOGF(INFO, "Loaded data for script '{}'. ArgCount={}, StackSize={}", name, data.ArgCount, data.StackSize);
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
				g_Pointers.KillGtaThread(thread); // thread->Kill();
				LOGF(INFO, "Killed thread with ID {}.", id);
			}
		}

		scriptThreadIds.clear();
		LoadScripts();
	}
}