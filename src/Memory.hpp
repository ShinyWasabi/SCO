#pragma once
#include "util/Joaat.hpp"

namespace SCOL
{
	class Memory
	{
	public:
		Memory(void* ptr = nullptr);
		explicit Memory(std::uintptr_t ptr);

		template<typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> As() const;

		template<typename T>
		std::enable_if_t<std::is_lvalue_reference_v<T>, T> As() const;

		template<typename T>
		std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> As() const;

		template<typename T>
		Memory Add(T offset) const;

		template<typename T>
		Memory Sub(T offset) const;

		Memory Rip() const;

		static std::optional<Memory> ScanPattern(const std::optional<std::uint8_t>* pattern, std::size_t length, Memory begin, std::size_t moduleSize);
		static std::optional<Memory> ScanPattern(const char* name, const char* pattern);

	private:
		void* m_Ptr;
		static inline std::unordered_map<joaat_t, std::optional<Memory>> m_CachedResults;
	};

	inline Memory::Memory(void* ptr) :
		m_Ptr(ptr)
	{
	}

	inline Memory::Memory(std::uintptr_t ptr) :
		m_Ptr(reinterpret_cast<void*>(ptr))
	{
	}

	template<typename T>
	inline std::enable_if_t<std::is_pointer_v<T>, T> Memory::As() const
	{
		return reinterpret_cast<T>(m_Ptr);
	}

	template<typename T>
	inline std::enable_if_t<std::is_lvalue_reference_v<T>, T> Memory::As() const
	{
		return *reinterpret_cast<std::add_pointer_t<std::remove_reference_t<T>>>(m_Ptr);
	}

	template<typename T>
	inline std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> Memory::As() const
	{
		return reinterpret_cast<std::uintptr_t>(m_Ptr);
	}

	template<typename T>
	inline Memory Memory::Add(T offset) const
	{
		return Memory(As<std::uintptr_t>() + offset);
	}

	template<typename T>
	inline Memory Memory::Sub(T offset) const
	{
		return Memory(As<std::uintptr_t>() - offset);
	}

	inline Memory Memory::Rip() const
	{
		return Add(As<std::int32_t&>()).Add(4);
	}
}