#pragma once

namespace SCOL
{
	class Settings
	{
	public:
		struct Variables
		{
			std::string ScriptsFolder = ".";
			int ReloadKey = VK_F5;
		};

		struct ScriptData
		{
			std::vector<std::uint64_t> Args;
			std::uint32_t ArgCount = 0;
			std::uint32_t StackSize = 1424;
			std::uint32_t CleanupFunction = 0;
		};

		static void Init(const std::string& file)
		{
			GetInstance().InitImpl(file);
		}

		static void Load()
		{
			GetInstance().LoadImpl();
		}

		static void Update()
		{
			GetInstance().UpdateImpl();
		}

		static ScriptData GetScriptData(const std::string& name)
		{
			return GetInstance().GetScriptDataImpl(name);
		}

	private:
		static Settings& GetInstance()
		{
			static Settings instance;
			return instance;
		}

		void InitImpl(const std::string& file);
		void LoadImpl();
		void UpdateImpl();
		ScriptData GetScriptDataImpl(const std::string& name);

		std::string m_FileName;
		std::chrono::time_point<std::chrono::steady_clock> m_LastUpdate;
	};

	inline Settings::Variables g_Variables{};
}