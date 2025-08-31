#pragma once
#include "pgBase.hpp"
#include "scrValue.hpp"

namespace rage
{
	class scrProgram : public pgBase
	{
	public:
		std::uint8_t** m_CodeBlocks;
		std::uint32_t m_GlobalVersion;
		std::uint32_t m_CodeSize;
		std::uint32_t m_ArgCount;
		std::uint32_t m_StaticCount;
		std::uint32_t m_GlobalCountAndBlock;
		std::uint32_t m_NativeCount;
		scrValue* m_Statics;
		scrValue** m_Globals;
		std::uint64_t* m_Natives;
		std::uint32_t m_ProcCount;
		char m_Pad1[0x04];
		const char** m_ProcNames;
		std::uint32_t m_NameHash;
		std::uint32_t m_RefCount;
		const char* m_Name;
		const char** m_Strings;
		std::uint32_t m_StringCount;
		char m_Breakpoints[0x0C];

		std::uint32_t GetNumCodeBlocks() const
		{
			return (m_CodeSize + 0x3FFF) >> 14;
		}

		std::uint32_t GetCodeBlockSize(std::uint32_t block) const
		{
			auto num = GetNumCodeBlocks();
			if (block < num)
			{
				if (block == num - 1)
					return (m_CodeSize & 0x3FFF);
				return 0x4000;
			}

			return 0;
		}

		std::uint8_t* GetCodeBlock(std::uint32_t block) const
		{
			return m_CodeBlocks[block];
		}

		std::uint8_t* GetCode(std::uint32_t index) const
		{
			if (index < m_CodeSize)
				return &m_CodeBlocks[index >> 14][index & 0x3FFF];

			return nullptr;
		}

		scrValue* GetStatic(std::uint32_t index) const
		{
			if (index < m_StaticCount)
				return &m_Statics[index];

			return nullptr;
		}

		std::uint32_t GetGlobalCount() const
		{
			return (m_GlobalCountAndBlock & 0x3FFFF);
		}

		std::uint32_t GetGlobalBlockIndex() const
		{
			return (m_GlobalCountAndBlock >> 0x12);
		}

		std::uint32_t GetNumGlobalBlocks() const
		{
			return ((m_GlobalCountAndBlock & 0x3FFFF) + 0x3FFF) >> 14;
		}

		std::uint32_t GetGlobalBlockSize(std::uint32_t block) const
		{
			auto num = GetNumGlobalBlocks();
			if (block < num)
			{
				if (block == num - 1)
					return (m_GlobalCountAndBlock & 0x3FFFF) - (block << 14);
				return 0x4000;
			}

			return 0;
		}

		scrValue* GetGlobalBlock(std::uint32_t block) const
		{
			return m_Globals[block];
		}

		scrValue* GetGlobal(std::uint32_t index) const
		{
			if (index < GetGlobalCount())
				return &m_Globals[index >> 0x12 & 0x3F][index & 0x3FFFF];

			return nullptr;
		}

		std::uint32_t GetNumStringBlocks() const
		{
			return (m_StringCount + 0x3FFF) >> 14;
		}

		std::uint32_t GetStringBlockSize(std::uint32_t block) const
		{
			auto num = GetNumStringBlocks();
			if (block < num)
			{
				if (block == num - 1)
					return (m_StringCount & 0x3FFF);
				return 0x4000;
			}

			return 0;
		}

		const char* GetStringBlock(std::uint32_t block) const
		{
			return m_Strings[block];
		}

		const char* GetString(std::uint32_t index) const
		{
			if (index < m_StringCount)
				return &m_Strings[index >> 14][index & 0x3FFF];

			return nullptr;
		}

		static scrProgram* FindScriptProgram(std::uint32_t hash);
	};
	static_assert(sizeof(scrProgram) == 0x80);
}