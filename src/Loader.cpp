#include "Loader.hpp"
#include "Hooking.hpp"
#include "Pointers.hpp"
#include "rage/scrThread.hpp"

namespace SCOL::Loader
{
	static CallHook createScriptThreadHook{};
	static std::string currentScriptPath{};
	static std::vector<std::uint32_t> scriptThreadIds;

	static std::uint32_t CreateScriptThreadHook(std::uint64_t hash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize)
	{
		Logger::Log("Loading script from path '{}'.", currentScriptPath);
		return g_Pointers.CreateScriptThread(currentScriptPath.c_str(), args, argCount, stackSize);
	}

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
			if (!entry.is_regular_file() || (entry.path().extension().string() != ".SCO" && entry.path().extension().string() != ".sco"))
			{
				// Logger::Log("Skipped invalid file: {}", entry.path().string());
				continue;
			}

			currentScriptPath = entry.path().string();

			if (!createScriptThreadHook)
			{
				createScriptThreadHook = CallSiteHook::AddHook(g_Pointers.CreateScriptThreadCaller, reinterpret_cast<PVOID>(CreateScriptThreadHook));
			}
			createScriptThreadHook->Enable();

			auto name = entry.path().filename().string();
			auto data = Settings::GetScriptData(name);
			Logger::Log("Loaded script data for '{}'. ArgCount={} StackSize={}", name, data.ArgCount, data.StackSize);
			if (auto id = g_Pointers.StartNewGtaThread(0x0000, data.Args.data(), data.ArgCount, data.StackSize))
			{
				Logger::Log("Started new thread with ID {}.", id);
				scriptThreadIds.push_back(id);
			}

			createScriptThreadHook->Disable();
			currentScriptPath.clear();

			std::this_thread::sleep_for(100ms);
		}
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