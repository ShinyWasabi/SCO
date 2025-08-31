#include "scrProgram.hpp"
#include "Pointers.hpp"

namespace rage
{
	scrProgram* scrProgram::FindScriptProgram(std::uint32_t hash)
	{
		for (int i = 0; i < 176; i++)
		{
			if (SCOL::g_Pointers.ScriptPrograms[i] && SCOL::g_Pointers.ScriptPrograms[i]->m_NameHash == hash)
				return SCOL::g_Pointers.ScriptPrograms[i];
		}

		return nullptr;
	}
}