#include "Loader.hpp"
#include "Hooking.hpp"
#include "Pointers.hpp"
#include "gta/GtaThread.hpp"

namespace SCOL::Loader
{
	static thread_local bool isCalledByUs{};
	static CallHook createScriptThreadHook{};
	static std::string currentScriptPath{};
	static std::vector<std::uint32_t> scriptThreadIds{};

	static std::uint32_t CreateScriptThreadHook(std::uint64_t hash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize)
	{
		if (isCalledByUs)
		{
			Logger::Log("Loading script from path '{}'.", currentScriptPath);
			return g_Pointers.CreateScriptThread(currentScriptPath.c_str(), args, argCount, stackSize);
		}

		auto original = createScriptThreadHook->GetOriginal<std::uint32_t(*)(std::int64_t, PVOID, std::uint32_t, std::uint32_t)>();
		return original(hash, args, argCount, stackSize);
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
			if (!entry.is_regular_file() || entry.path().extension().string() != ".sco")
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
			isCalledByUs = true;

			auto name = entry.path().stem().string();
			auto data = Settings::GetScriptData(name);
			Logger::Log("Loaded script data for '{}'. ArgCount={}, StackSize={}", name, data.ArgCount, data.StackSize);
			if (auto id = g_Pointers.StartNewGtaThread(Joaat(name), data.Args.data(), data.ArgCount, data.StackSize))
			{
				Logger::Log("Started new thread with ID {}.", id);
				scriptThreadIds.push_back(id);
			}

			createScriptThreadHook->Disable();
			currentScriptPath.clear();
			isCalledByUs = false;

			std::this_thread::sleep_for(100ms);
		}
	}

	void ReloadAllScripts()
	{
		for (auto id : scriptThreadIds)
		{
			if (auto thread = static_cast<GtaThread*>(rage::scrThread::FindScriptThreadById(id)))
			{
				g_Pointers.KillGtaThread(thread); // thread->Kill();
				Logger::Log("Killed thread with ID {}.", id);
			}
		}

		scriptThreadIds.clear();
		LoadAllScripts();
	}
}