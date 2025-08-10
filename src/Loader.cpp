#include "Loader.hpp"
#include "Pointers.hpp"
#include "gta/GtaThread.hpp"
#include "rage/atArray.hpp"

namespace SCOL::Loader
{
	static std::vector<std::uint32_t> scriptThreadIds{};

	void LoadAllScripts()
	{
		auto scriptsFolder = std::filesystem::absolute(g_Variables.ScriptsFolder);

		if (!std::filesystem::exists(scriptsFolder) || !std::filesystem::is_directory(scriptsFolder))
		{
			Logger::Log("Scripts folder is invalid: {}", scriptsFolder.string());
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(scriptsFolder))
		{
			if (!entry.is_regular_file() || entry.path().extension().string() != ".sco")
				continue;

			auto path = entry.path().string();
			auto name = entry.path().stem().string();
			auto data = Settings::GetScriptData(name);
			Logger::Log("Loaded script data for '{}'. ArgCount={}, StackSize={}", name, data.ArgCount, data.StackSize);
			if (auto id = g_Pointers.CreateScriptThread(path.c_str(), data.Args.data(), data.ArgCount, data.StackSize))
			{
				if (auto thread = reinterpret_cast<GtaThread*>(rage::scrThread::FindScriptThreadById(id)))
				{
					g_Pointers.RegisterScriptHandler(g_Pointers.ScriptHandlerMgrPtr, thread);

					scriptThreadIds.push_back(id);
					Logger::Log("Started new thread with ID {}.", id);
				}
			}
		}
	}

	void ReloadAllScripts()
	{
		if (!g_Pointers.ScriptThreads->size() || rage::scrThread::FindScriptThread("landing_pre_startup"_J) || rage::scrThread::FindScriptThread("Startup"_J))
		{
			Logger::Log("Not safe to reload scripts at the moment.");
			return;
		}

		for (auto id : scriptThreadIds)
		{
			if (auto thread = reinterpret_cast<GtaThread*>(rage::scrThread::FindScriptThreadById(id)))
			{
				g_Pointers.KillGtaThread(thread); // thread->Kill();
				Logger::Log("Killed thread with ID {}.", id);
			}
		}

		scriptThreadIds.clear();
		LoadAllScripts();
	}
}