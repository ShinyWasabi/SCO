#pragma once

namespace SCO
{
    class Logger
    {
    public:
        Logger() :
            m_FileName("SCO.log")
        {
            std::ofstream file(m_FileName, std::ofstream::out | std::ofstream::trunc);
            file.close();
        }

        template<typename... Args>
        static void Log(std::format_string<Args...> fmt, Args&&... args)
        {
            GetInstance().LogImpl(std::format(fmt, std::forward<Args>(args)...));
        }

    private:
        static Logger& GetInstance()
        {
            static Logger instance;
            return instance;
        }

        void LogImpl(const std::string& message)
        {
            std::ofstream file(m_FileName, std::ios::app);
            if (!file.is_open())
                return;

            auto now = std::chrono::system_clock::now();
            auto local = std::chrono::zoned_time(std::chrono::current_zone(), now);
            file << "[" << std::format("{0:%H:%M:%S}", local) << "] " << message << "\n";
            file.close();
        }

        std::string m_FileName;
    };
}