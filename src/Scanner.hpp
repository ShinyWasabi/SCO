#pragma once
#include "util/Memory.hpp"

namespace SCOL
{
	using ScanFunc = std::function<void(Memory)>;

	class Scanner
	{
	public:
		void Add(const char* name, const char* pattern, const ScanFunc& func);
		bool Scan();

		static std::optional<Memory> ScanPattern(const std::optional<std::uint8_t>* pattern, std::size_t length, Memory begin, std::size_t moduleSize);
		static std::optional<Memory> ScanPattern(const char* name, const char* pattern);

	private:
		struct Pattern
		{
			std::string m_Name;
			std::string m_Pattern;
			ScanFunc m_Func;
		};

		std::vector<Pattern> m_Patterns;
		static inline std::unordered_map<joaat_t, std::optional<Memory>> m_CachedResults;
	};
}