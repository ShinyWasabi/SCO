#include "Logging.hpp"

namespace SCOL
{
	static std::string FormatFile(const LogMessagePtr msg)
	{
		std::stringstream out;

		constexpr std::array<const char*, 4> levelStrings = { "DEBUG", "INFO", "WARN", "ERROR" };

		const auto timestamp = std::format("{0:%H:%M:%S}", msg->Timestamp());
		const auto& location = msg->Location();
		const auto level = msg->Level();
		const auto file = std::filesystem::path(location.file_name()).filename().string();

		out << "[" << timestamp << "]" << "[" << levelStrings[level] << "/" << file << ":" << location.line() << "] " << msg->Message();

		return out.str();
	}

	void Logging::InitImpl(const std::filesystem::path& file)
	{
		m_FileOut.open(file, std::ios::out | std::ios::trunc);

		Logger::Init();
		Logger::AddSink([this](LogMessagePtr msg) {
			m_FileOut << FormatFile(msg);
			m_FileOut.flush();
		});
	}
}