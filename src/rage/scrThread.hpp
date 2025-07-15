#pragma once

namespace rage
{
	class scrThread
	{
	public:
		enum class State
		{
			IDLE,
			RUNNING,
			KILLED,
			PAUSED,
			UNK4
		};

		class Context
		{
		public:
			std::uint32_t m_ThreadId;
			std::uint64_t m_ScriptHash;
			State m_State;
			std::uint32_t m_ProgramCounter;
			std::uint32_t m_FramePointer;
			std::uint32_t m_StackPointer;
			float m_TimerA;
			float m_TimerB;
			float m_WaitTimer;
			char m_padding1[0x2C];
			std::uint32_t m_StackSize;
			char m_Pad[0x54];
		};
		static_assert(sizeof(Context) == 0xB0);


		virtual ~scrThread() = default;
		virtual void Reset(std::uint64_t script_hash, void* args, std::uint32_t arg_count) = 0;
		virtual State RunImpl() = 0;
		virtual State Run() = 0;
		virtual void Kill() = 0;
		virtual void GetInfo(void* info) = 0;

		Context m_Context;
		void* m_Stack;
		char m_Pad[0x4];
		std::uint32_t m_ParameterSize;
		std::uint32_t m_ParameterLoc;
		char m_Pad2[0x4];
		char m_ErrorMessage[128];
		std::uint32_t m_ScriptHash;
		char m_ScriptName[64];

		static rage::scrThread* FindScriptThread(std::uint32_t hash);
		static rage::scrThread* FindScriptThreadById(std::uint32_t id);
	};
	static_assert(sizeof(scrThread) == 0x198);
}