#include "Engine.hpp"

namespace rage
{
	rage::scrThread* FindScriptThread(atArray<scrThread*>* threads, std::uint32_t hash)
	{
		for (auto& thread : *threads)
		{
			if (thread && thread->m_Context.m_ThreadId && thread->m_ScriptHash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	rage::scrThread* FindScriptThreadById(atArray<scrThread*>* threads, std::uint32_t id)
	{
		for (auto& thread : *threads)
		{
			if (thread && thread->m_Context.m_ThreadId == id)
			{
				return thread;
			}
		}

		return nullptr;
	}
}