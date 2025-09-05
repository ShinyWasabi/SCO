#pragma once
#include "scrValue.hpp"
#include "scrVector.hpp"

namespace rage
{
	class scrNativeCallContext
	{
	public:
		scrNativeCallContext(scrValue* retVal, std::int32_t argCount, scrValue* args) :
			m_ReturnValue(retVal),
			m_ArgCount(argCount),
			m_Args(args),
			m_NumVectorRefs(0)
		{
		}

		scrValue* m_ReturnValue;
		std::uint32_t m_ArgCount;
		scrValue* m_Args;
		std::int32_t m_NumVectorRefs;
		scrVector* m_VectorRefTargets[4];
		Vector3 m_VectorRefSources[4];
	};
	static_assert(sizeof(scrNativeCallContext) == 0x80);

	using scrNativeHash = std::uint64_t;
	using scrNativeHandler = void (*)(scrNativeCallContext*);
}