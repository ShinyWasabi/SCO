#pragma once

namespace SCOL
{
	class Logging
	{
	public:
		static void Init(const std::filesystem::path& file)
		{
			return GetInstance().InitImpl(file);
		}

	private:
		static Logging& GetInstance()
		{
			static Logging instance{};
			return instance;
		}

		void InitImpl(const std::filesystem::path& file);

		std::ofstream m_FileOut;
	};
}