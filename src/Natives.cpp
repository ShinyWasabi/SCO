#include "Natives.hpp"
#include "Pointers.hpp"
#include "Scanner.hpp"
#include "rage/scrThread.hpp"
#include "rage/tlsContext.hpp"

namespace SCOL::Natives
{
	// TO-DO: If we set a static to a literal string, it will point to invalid memory next time we reload the script as our string table will be freed.
	// NOTE: Globals cannot be declared as STRING, so we don't have SET/GET methods for it.

	static std::unordered_map<joaat_t, std::ofstream> scriptLogs;

	static constexpr rage::scrNativeHash WRITE_MEMORY                  = 0xEEE74A05DE4C2A07;
	static constexpr rage::scrNativeHash READ_MEMORY                   = 0x1E9F7F45D0E77AAC;
	static constexpr rage::scrNativeHash SET_CURRENT_SCRIPT_THREAD     = 0x7AFACDB81809E2C1;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_SET_INT         = 0x857A2700DC9407CF;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_SET_FLOAT       = 0xD8AABF55B8C2ABCC;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_SET_STRING      = 0xBBED7EBD9CB32457;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_SET_TEXT_LABEL  = 0x82486D97F828522B;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_SET_VECTOR      = 0x1CBDE8A15884D019;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_GET_INT         = 0xE60444296FBC9C3C;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_GET_FLOAT       = 0x02917A468A9F9203;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_GET_STRING      = 0x50AECCD9E4A23B3A;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_GET_TEXT_LABEL  = 0x83C64F9F51CCB284;
	static constexpr rage::scrNativeHash SCRIPT_STATIC_GET_VECTOR      = 0x022347480BDA5340;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_SET_INT         = 0xE308F800129466D7;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_SET_FLOAT       = 0xCE23B2BC1A4037EE;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_SET_TEXT_LABEL  = 0xC7540C6E3F588A63;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_SET_VECTOR      = 0x2D670A4779E7390E;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_GET_INT         = 0x95257993CA3F052E;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_GET_FLOAT       = 0xDC21946AD79CBEAF;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_GET_TEXT_LABEL  = 0x79516115C6DB77F2;
	static constexpr rage::scrNativeHash SCRIPT_GLOBAL_GET_VECTOR      = 0x673E810DC3EC1307;
	static constexpr rage::scrNativeHash LOG_TO_FILE                   = 0x7F41C15A89FDEE9F;

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
		auto name = ctx->m_Args[0].String;
		auto pattern = ctx->m_Args[1].String;
		auto offset = ctx->m_Args[2].Int;
		auto rip = ctx->m_Args[3].Int;
		auto patch = ctx->m_Args[4].Reference;
		auto protect = ctx->m_Args[5].Int;

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
		auto name = ctx->m_Args[0].String;
		auto pattern = ctx->m_Args[1].String;
		auto offset = ctx->m_Args[2].Int;
		auto rip = ctx->m_Args[3].Int;

		std::int32_t retVal{};
		if (auto addr = Scanner::ScanPattern(name, pattern))
		{
			auto loc = addr->Add(offset);
			if (rip)
				loc = loc.Rip();

			retVal = loc.As<std::int32_t&>();
		}

		ctx->m_ReturnValue->Int = retVal;
	}

	static void NativeCommandSetCurrentScriptThread(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			rage::tlsContext::Get()->m_CurrentScriptThread = thread;
			rage::tlsContext::Get()->m_ScriptThreadActive = true;
		}
	}

	static void NativeCommandScriptStaticSetInt(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;
		auto value = ctx->m_Args[2].Int;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			thread->m_Stack[index].Int = value;
		}
	}

	static void NativeCommandScriptStaticSetFloat(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;
		auto value = ctx->m_Args[2].Float;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			thread->m_Stack[index].Float = value;
		}
	}

	static void NativeCommandScriptStaticSetString(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;
		auto value = ctx->m_Args[2].String;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			thread->m_Stack[index].String = value;
		}
	}

	static void NativeCommandScriptStaticSetTextLabel(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;
		auto value = ctx->m_Args[2].String;
		auto size = ctx->m_Args[3].Int;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			auto dest = reinterpret_cast<char*>(&thread->m_Stack[index]);
			std::strncpy(dest, value, size - 1);
			dest[size - 1] = '\0';
		}
	}

	static void NativeCommandScriptStaticSetVector(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;
		auto valueX = ctx->m_Args[2].Float;
		auto valueY = ctx->m_Args[3].Float;
		auto valueZ = ctx->m_Args[4].Float;

		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			thread->m_Stack[index + 0].Float = valueX;
			thread->m_Stack[index + 1].Float = valueY;
			thread->m_Stack[index + 2].Float = valueZ;
		}
	}

	static void NativeCommandScriptStaticGetInt(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;

		std::int32_t retVal{};
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			retVal = thread->m_Stack[index].Int;
		}

		ctx->m_ReturnValue->Int = retVal;
	}

	static void NativeCommandScriptStaticGetFloat(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;

		float retVal{};
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			retVal = thread->m_Stack[index].Float;
		}

		ctx->m_ReturnValue->Float = retVal;
	}

	static void NativeCommandScriptStaticGetString(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;

		const char* retVal{""};
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			retVal = thread->m_Stack[index].String;
		}

		ctx->m_ReturnValue->String = retVal;
	}

	static void NativeCommandScriptStaticGetTextLabel(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;

		char* retVal{};
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			retVal = reinterpret_cast<char*>(&thread->m_Stack[index]);
		}

		ctx->m_ReturnValue->String = retVal ? retVal : "";
	}

	static void NativeCommandScriptStaticGetVector(rage::scrNativeCallContext* ctx)
	{
		auto hash = ctx->m_Args[0].Int;
		auto index = ctx->m_Args[1].Int;

		rage::Vector3 retVal{};
		if (auto thread = rage::scrThread::FindScriptThread(hash))
		{
			retVal.x = thread->m_Stack[index + 0].Float;
			retVal.y = thread->m_Stack[index + 1].Float;
			retVal.z = thread->m_Stack[index + 2].Float;
		}

		ctx->m_ReturnValue[0].Float = retVal.x;
		ctx->m_ReturnValue[1].Float = retVal.y;
		ctx->m_ReturnValue[2].Float = retVal.z;
	}

	static void NativeCommandScriptGlobalSetInt(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;
		auto value = ctx->m_Args[1].Int;

		g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Int = value;
	}

	static void NativeCommandScriptGlobalSetFloat(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;
		auto value = ctx->m_Args[1].Float;

		g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Float = value;
	}

	static void NativeCommandScriptGlobalSetTextLabel(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;
		auto value = ctx->m_Args[1].String;
		auto size = ctx->m_Args[2].Int;

		auto dest = reinterpret_cast<char*>(&g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF]);
		std::strncpy(dest, value, size - 1);
		dest[size - 1] = '\0';
	}

	static void NativeCommandScriptGlobalSetVector(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;
		auto valueX = ctx->m_Args[1].Float;
		auto valueY = ctx->m_Args[2].Float;
		auto valueZ = ctx->m_Args[3].Float;

		g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 0].Float = valueX;
		g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 1].Float = valueY;
		g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 2].Float = valueZ;
	}

	static void NativeCommandScriptGlobalGetInt(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;

		ctx->m_ReturnValue->Int = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Int;
	}

	static void NativeCommandScriptGlobalGetFloat(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;

		ctx->m_ReturnValue->Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Float;
	}

	static void NativeCommandScriptGlobalGetTextLabel(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;

		ctx->m_ReturnValue->String = reinterpret_cast<char*>(&g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF]);
	}

	static void NativeCommandScriptGlobalGetVector(rage::scrNativeCallContext* ctx)
	{
		auto index = ctx->m_Args[0].Int;

		ctx->m_ReturnValue[0].Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 0].Float;
		ctx->m_ReturnValue[1].Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 1].Float;
		ctx->m_ReturnValue[2].Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 2].Float;
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
				logFile << std::format("{}", args->Int);
				break;
			case 1:
				logFile << std::format("{:.2f}", args->Float);
				break;
			case 2:
				logFile << std::format("{}", args->String ? args->String : "NULL");
				break;
			case 3:
				logFile << std::format("<<{:.4f}, {:.4f}, {:.4f}>>", args->Reference[0].Float, args->Reference[1].Float, args->Reference[2].Float);
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

		RegisterNative(WRITE_MEMORY,                 NativeCommandWriteMemory);
		RegisterNative(READ_MEMORY,                  NativeCommandReadMemory);
		RegisterNative(SET_CURRENT_SCRIPT_THREAD,    NativeCommandSetCurrentScriptThread);
		RegisterNative(SCRIPT_STATIC_SET_INT,        NativeCommandScriptStaticSetInt);
		RegisterNative(SCRIPT_STATIC_SET_FLOAT,      NativeCommandScriptStaticSetFloat);
		RegisterNative(SCRIPT_STATIC_SET_STRING,     NativeCommandScriptStaticSetString);
		RegisterNative(SCRIPT_STATIC_SET_TEXT_LABEL, NativeCommandScriptStaticSetTextLabel);
		RegisterNative(SCRIPT_STATIC_SET_VECTOR,     NativeCommandScriptStaticSetVector);
		RegisterNative(SCRIPT_STATIC_GET_INT,        NativeCommandScriptStaticGetInt);
		RegisterNative(SCRIPT_STATIC_GET_FLOAT,      NativeCommandScriptStaticGetFloat);
		RegisterNative(SCRIPT_STATIC_GET_STRING,     NativeCommandScriptStaticGetString);
		RegisterNative(SCRIPT_STATIC_GET_TEXT_LABEL, NativeCommandScriptStaticGetTextLabel);
		RegisterNative(SCRIPT_STATIC_GET_VECTOR,     NativeCommandScriptStaticGetVector);
		RegisterNative(SCRIPT_GLOBAL_SET_INT,        NativeCommandScriptGlobalSetInt);
		RegisterNative(SCRIPT_GLOBAL_SET_FLOAT,      NativeCommandScriptGlobalSetFloat);
		RegisterNative(SCRIPT_GLOBAL_SET_TEXT_LABEL, NativeCommandScriptGlobalSetTextLabel);
		RegisterNative(SCRIPT_GLOBAL_SET_VECTOR,     NativeCommandScriptGlobalSetVector);
		RegisterNative(SCRIPT_GLOBAL_GET_INT,        NativeCommandScriptGlobalGetInt);
		RegisterNative(SCRIPT_GLOBAL_GET_FLOAT,      NativeCommandScriptGlobalGetFloat);
		RegisterNative(SCRIPT_GLOBAL_GET_TEXT_LABEL, NativeCommandScriptGlobalGetTextLabel);
		RegisterNative(SCRIPT_GLOBAL_GET_VECTOR,     NativeCommandScriptGlobalGetVector);
		RegisterNative(LOG_TO_FILE,                  NativeCommandLogToFile);
	}
}