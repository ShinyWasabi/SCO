#pragma once
#include "scrVector.hpp"

namespace rage
{
	class scrNativeCallContext
	{
	public:
		constexpr void reset()
		{
			m_ArgCount = 0;
			m_NumVectorRefs = 0;
		}

		template<typename T>
		constexpr void PushArg(T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(m_Args) + (m_ArgCount++)) = std::forward<T>(value);
		}

		template<typename T>
		constexpr T& GetArg(std::size_t index)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			return *reinterpret_cast<T*>(reinterpret_cast<std::uint64_t*>(m_Args) + index);
		}

		template<typename T>
		constexpr void SetArg(std::size_t index, T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(m_Args) + index) = std::forward<T>(value);
		}

		template<typename T>
		constexpr T* GetReturnValue()
		{
			return reinterpret_cast<T*>(m_ReturnValue);
		}

		template<typename T>
		constexpr void SetReturnValue(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(m_ReturnValue) = std::forward<T>(value);
		}

		void FixVectors()
		{
			for (int i = 0; i < m_NumVectorRefs; i++)
			{
				*m_VectorRefTargets[i] = m_VectorRefSources[i];
			}
			m_NumVectorRefs = 0;
		}

		rage::vector3& GetSourceVector(int index)
		{
			return m_VectorRefSources[index];
		}

	protected:
		void* m_ReturnValue;
		std::uint32_t m_ArgCount;
		void* m_Args;
		std::int32_t m_NumVectorRefs;
		rage::scrVector* m_VectorRefTargets[4];
		rage::vector3 m_VectorRefSources[4];
	};
	static_assert(sizeof(scrNativeCallContext) == 0x80);

	using scrNativeHash = std::uint64_t;
	using scrNativePair = std::pair<scrNativeHash, scrNativeHash>;
	using scrNativeHandler = void (*)(scrNativeCallContext*);
}