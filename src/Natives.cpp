#include "Natives.hpp"
#include "Pointers.hpp"
#include "Scanner.hpp"
#include "rage/scrThread.hpp"
#include "rage/tlsContext.hpp"

namespace SCOL::Natives
{
	static std::unordered_map<joaat_t, std::ofstream> scriptLogs;

	static constexpr rage::scrNativeHash WRITE_MEMORY = 0xEEE74A05DE4C2A07;
	static constexpr rage::scrNativeHash READ_MEMORY = 0x1E9F7F45D0E77AAC;
	static constexpr rage::scrNativeHash SET_CURRENT_SCRIPT_THREAD = 0x7AFACDB81809E2C1;
	static constexpr rage::scrNativeHash WRITE_SCRIPT_STATIC = 0xBB3346E0994B7EA5;
	static constexpr rage::scrNativeHash READ_SCRIPT_STATIC = 0x8725A6C8DE144DBC;
	static constexpr rage::scrNativeHash LOG_TO_FILE = 0x7F41C15A89FDEE9F;

	static void CleanupScriptLog(joaat_t scriptHash)
	{
		auto it = scriptLogs.find(scriptHash);
		if (it != scriptLogs.end())
		{
			it->second.close();
			scriptLogs.erase(it);
			LOGF(INFO, "Closed log file for script with hash 0x{:X}.", scriptHash);
		}
	}

	static void NativeCommandWriteMemory(rage::scrNativeCallContext* ctx)
	{
		auto name = ctx->GetArg(0).String;
		auto pattern = ctx->GetArg(1).String;
		auto offset = ctx->GetArg(2).Int;
		auto rip = ctx->GetArg(3).Int;
		auto patch = ctx->GetArg(4).Reference;
		auto protect = ctx->GetArg(5).Int;

		if (auto addr = Scanner::ScanPattern(name, pattern))
		{
			auto loc = addr->Add(offset);
			if (rip)
				loc = loc.Rip();

			auto ptr = loc.As<std::uint8_t*>();

			auto count = static_cast<int>(patch[0].Any);
			auto items = patch + 1;

			std::vector<std::uint8_t> data;
			for (int i = 0; i < count; i++)
			{
				data.push_back(static_cast<std::uint8_t>(items[i].Any));
			}

			if (protect)
			{
				DWORD oldProtect, temp;
				VirtualProtect(ptr, count, PAGE_EXECUTE_READWRITE, &oldProtect);
				std::memcpy(ptr, data.data(), count);
				VirtualProtect(ptr, count, oldProtect, &temp);
			}
			else
			{
				std::memcpy(ptr, data.data(), count);
			}
		}
	}

	static void NativeCommandReadMemory(rage::scrNativeCallContext* ctx)
	{
		auto name = ctx->GetArg(0).String;
		auto pattern = ctx->GetArg(1).String;
		auto offset = ctx->GetArg(2).Int;
		auto rip = ctx->GetArg(3).Int;

		rage::scrValue retVal{};
		if (auto addr = Scanner::ScanPattern(name, pattern))
		{
			auto loc = addr->Add(offset);
			if (rip)
				loc = loc.Rip();

			retVal.Int = loc.As<std::int32_t&>();
		}

		ctx->SetReturnValue(retVal);
	}

	static void NativeCommandSetCurrentScriptThread(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->GetArg(0).Int;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			rage::tlsContext::Get()->m_CurrentScriptThread = thread;
			rage::tlsContext::Get()->m_ScriptThreadActive = true; // What if it was false?
		}
	}

	static void NativeCommandWriteScriptStatic(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->GetArg(0).Int;
		auto index = ctx->GetArg(1).Int;
		auto value = ctx->GetArg(2).Int;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			auto addr = reinterpret_cast<std::uintptr_t*>((std::uintptr_t)thread->m_Stack + (index * sizeof(rage::scrValue)));
			if (addr)
			{
				*addr = value;
			}
		}
	}

	static void NativeCommandReadScriptStatic(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->GetArg(0).Int;
		auto index = ctx->GetArg(1).Int;

		rage::scrValue retVal{};
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			auto addr = reinterpret_cast<std::uintptr_t*>((std::uintptr_t)thread->m_Stack + (index * sizeof(rage::scrValue)));
			if (addr)
			{
				retVal.Int = *reinterpret_cast<int*>(addr);
			}
		}

		ctx->SetReturnValue(retVal);
	}

	static void NativeCommandLogToFile(rage::scrNativeCallContext* ctx)
	{
		auto hash = rage::tlsContext::Get()->m_CurrentScriptThread->m_ScriptHash;
		auto name = rage::tlsContext::Get()->m_CurrentScriptThread->m_ScriptName;

		auto it = scriptLogs.find(hash);
		if (it == scriptLogs.end())
		{
			auto path = std::filesystem::path(g_Variables.ScriptsFolder) / (std::string(name) + ".log");
			it = scriptLogs.emplace(hash, std::ofstream{path, std::ios::out | std::ios::trunc}).first;
		}

		auto& logFile = it->second;

		auto argCount = ctx->m_ArgCount - 1;
		auto args = ctx->m_Args + 1;
		auto typeMask = args[-1].Int;

		while (argCount > 0)
		{
			switch (typeMask & 3)
			{
			case 0:
				logFile << args->Int;
				break;
			case 1:
				logFile << args->Float;
				break;
			case 2:
				logFile << args->String;
				break;
			case 3:
				logFile << "<<" << args->Reference[0].Float << ", " << args->Reference[1].Float << ", " << args->Reference[2].Float << ">>";
				break;
			}

			++args;
			typeMask >>= 2;
			--argCount;
		}

		logFile << std::endl;
		logFile.flush();
	}

	void RegisterNatives()
	{
		static auto RegisterNative = [](rage::scrNativeHash hash, rage::scrNativeHandler handler) {
			g_Pointers.RegisterNativeCommand(g_Pointers.NativeRegistrationTable, hash, handler);
			LOGF(INFO, "Registered native command with hash 0x{:X}.", hash);
		};

		RegisterNative(WRITE_MEMORY, NativeCommandWriteMemory);
		RegisterNative(READ_MEMORY, NativeCommandReadMemory);
		RegisterNative(SET_CURRENT_SCRIPT_THREAD, NativeCommandSetCurrentScriptThread);
		RegisterNative(WRITE_SCRIPT_STATIC, NativeCommandWriteScriptStatic);
		RegisterNative(READ_SCRIPT_STATIC, NativeCommandReadScriptStatic);
		RegisterNative(LOG_TO_FILE, NativeCommandLogToFile);
	}
}