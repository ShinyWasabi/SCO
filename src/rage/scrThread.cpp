#include "scrThread.hpp"
#include "atArray.hpp"
#include "../Pointers.hpp"

namespace rage
{
	rage::scrThread* scrThread::FindScriptThread(std::uint32_t hash)
	{
		for (auto& thread : *SCO::g_Pointers.ScriptThreads)
		{
			if (thread && thread->m_Context.m_ThreadId && thread->m_ScriptHash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	rage::scrThread* scrThread::FindScriptThreadById(std::uint32_t id)
	{
		for (auto& thread : *SCO::g_Pointers.ScriptThreads)
		{
			if (thread && thread->m_Context.m_ThreadId == id)
			{
				return thread;
			}
		}

		return nullptr;
	}
}