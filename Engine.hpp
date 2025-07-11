#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>

namespace rage
{
	template<typename T>
	class atArray
	{
	public:
		atArray() :
			m_Data(nullptr),
			m_Size(0),
			m_Capacity(0)
		{
		}

		T* begin() const
		{
			return &m_Data[0];
		}

		T* end() const
		{
			return &m_Data[m_Size];
		}

		T* data() const
		{
			return m_Data;
		}

		std::uint16_t size() const
		{
			return m_Size;
		}

		std::uint16_t capacity() const
		{
			return m_Capacity;
		}

		T& operator[](std::uint16_t index) const
		{
			return m_Data[index];
		}

		bool contains(T comparator)
		{
			for (auto iter_value : this)
			{
				if (iter_value == comparator)
				{
					return true;
				}
			}
			return false;
		}

	public:
		T* m_Data;
		std::uint16_t m_Size;
		std::uint16_t m_Capacity;
	};
	static_assert(sizeof(rage::atArray<std::uint32_t>) == 0x10);

	class scrNativeCallContext
	{
	public:
		template<typename T>
		constexpr T& GetArg(std::size_t index)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			return *reinterpret_cast<T*>(reinterpret_cast<std::uint64_t*>(m_Args) + index);
		}

		template<typename T>
		constexpr void SetReturnValue(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(m_ReturnValue) = std::forward<T>(value);
		}

	protected:
		void* m_ReturnValue;
		std::uint32_t m_ArgCount;
		void* m_Args;
		char m_Pad[0x68];
	};
	static_assert(sizeof(scrNativeCallContext) == 0x80);

	using scrNativeHash    = std::uint64_t;
	using scrNativeHandler = void (*)(scrNativeCallContext*);

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
	};
	static_assert(sizeof(scrThread) == 0x198);

	extern rage::scrThread* FindScriptThread(atArray<scrThread*>* threads, std::uint32_t hash);
	extern rage::scrThread* FindScriptThreadById(atArray<scrThread*>* threads, std::uint32_t id);
}