#include "Settings.hpp"
#include "util/SimpleIni.h"

namespace SCOL
{
	static std::vector<std::uint64_t> ParseArgs(const std::string& str)
	{
		std::vector<std::uint64_t> args;
		std::istringstream iss(str);
		std::string token;

		while (std::getline(iss, token, ','))
		{
			try
			{
				args.push_back(std::stoull(token));
			}
			catch (...)
			{
			}
		}

		return args;
	}

	Settings::Settings() :
		m_FileName("SCOL.ini")
	{
	}

	void Settings::LoadImpl()
	{
		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(m_FileName.c_str());

		bool dirty = false;

		if (!ini.GetValue("Settings", "ScriptsFolder", nullptr))
		{
			ini.SetValue("Settings", "ScriptsFolder", ".");
			dirty = true;
		}

		if (!ini.GetValue("Settings", "ReloadKey", nullptr))
		{
			ini.SetLongValue("Settings", "ReloadKey", VK_F5);
			dirty = true;
		}

		if (dirty)
			ini.SaveFile(m_FileName.c_str());

		g_Variables.ScriptsFolder = ini.GetValue("Settings", "ScriptsFolder", ".");
		g_Variables.ReloadKey = ini.GetLongValue("Settings", "ReloadKey", VK_F5);
	}

	void Settings::UpdateImpl()
	{
		const auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - m_LastUpdate) < 3s)
			return;

		m_LastUpdate = now;

		Load();
	}

	Settings::ScriptData Settings::GetScriptDataImpl(const std::string& name)
	{
		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(m_FileName.c_str());

		bool dirty = false;

		if (!ini.GetValue(name.c_str(), "Args", nullptr))
		{
			ini.SetValue(name.c_str(), "Args", "0");
			dirty = true;
		}

		if (!ini.GetValue(name.c_str(), "ArgCount", nullptr))
		{
			ini.SetLongValue(name.c_str(), "ArgCount", 0);
			dirty = true;
		}

		if (!ini.GetValue(name.c_str(), "StackSize", nullptr))
		{
			ini.SetLongValue(name.c_str(), "StackSize", 1424);
			dirty = true;
		}

		if (dirty)
			ini.SaveFile(m_FileName.c_str());

		ScriptData data;
		data.Args = ParseArgs(ini.GetValue(name.c_str(), "Args", "0"));
		data.ArgCount = ini.GetLongValue(name.c_str(), "ArgCount", 0);
		data.StackSize = ini.GetLongValue(name.c_str(), "StackSize", 1424);

		return data;
	}
}