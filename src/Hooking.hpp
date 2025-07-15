#pragma once
#include "Memory.hpp"

namespace SCO
{
	class CallHookMemory
	{
		static CallHookMemory& GetInstance()
		{
			static CallHookMemory instance;
			return instance;
		}

		PVOID AllocateJumpSequenceImpl(PVOID func);

		Memory m_Memory;
		int m_Offset;

	public:
		CallHookMemory();

		static PVOID AllocateJumpSequence(PVOID func)
		{
			return GetInstance().AllocateJumpSequenceImpl(func);
		}
	};

	class CallSiteHook
	{
	public:
		class Hook
		{
			Hook(Hook&& that) = delete;
			Hook& operator=(Hook&& that) = delete;
			Hook(Hook const&) = delete;
			Hook& operator=(Hook const&) = delete;

			PVOID m_Location;
			PVOID m_Hook;
			std::uint8_t m_PatchedBytes[5];
			std::uint8_t m_OriginalBytes[5];
			PVOID m_OriginalFunction;
			bool m_Enabled;

		public:
			Hook(PVOID location, PVOID hook);

			void Enable();
			void Disable();

			template<typename T>
			T GetOriginal()
			{
				return reinterpret_cast<T>(m_OriginalFunction);
			}
		};

		static std::shared_ptr<Hook> AddHook(PVOID location, PVOID hook)
		{
			return GetInstance().AddHookImpl(location, hook);
		}

	private:
		static CallSiteHook& GetInstance()
		{
			static CallSiteHook Instance;
			return Instance;
		}

		std::shared_ptr<Hook> AddHookImpl(PVOID location, PVOID hook);

		static inline std::vector<std::shared_ptr<Hook>> m_Hooks;
	};

	using CallHook = std::shared_ptr<CallSiteHook::Hook>;
}