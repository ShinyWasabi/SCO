#pragma once

namespace SCOL
{
    class IniFile
    {
    public:
        using Section = std::unordered_map<std::string, std::string>;

        bool LoadFile(const std::string& filename)
        {
            m_Sections.clear();

            std::ifstream file(filename);
            if (!file.is_open())
                return false;

            std::string line, currentSection;
            while (std::getline(file, line))
            {
                line = Trim(line);
                if (line.empty() || line[0] == ';' || line[0] == '#')
                    continue;

                if (line.front() == '[' && line.back() == ']')
                {
                    currentSection = line.substr(1, line.size() - 2);
                }
                else
                {
                    auto eqPos = line.find('=');
                    if (eqPos != std::string::npos)
                    {
                        std::string key = Trim(line.substr(0, eqPos));
                        std::string value = Trim(line.substr(eqPos + 1));
                        m_Sections[currentSection][key] = value;
                    }
                }
            }

            m_Filename = filename;
            return true;
        }

        bool SaveFile(const std::string& filename = "")
        {
            std::ofstream file(filename.empty() ? m_Filename : filename);
            if (!file.is_open())
                return false;

            for (auto& [sectionName, section] : m_Sections)
            {
                file << "[" << sectionName << "]\n";
                for (auto& [key, value] : section)
                {
                    file << key << "=" << value << "\n";
                }
                file << "\n";
            }
            return true;
        }

        const char* GetValue(const std::string& section, const std::string& key, const char* defaultValue = nullptr)
        {
            auto secIt = m_Sections.find(section);
            if (secIt != m_Sections.end())
            {
                auto keyIt = secIt->second.find(key);
                if (keyIt != secIt->second.end())
                    return keyIt->second.c_str();
            }
            return defaultValue;
        }

        long GetLongValue(const std::string& section, const std::string& key, long defaultValue = 0)
        {
            const char* val = GetValue(section, key, nullptr);
            if (val)
            {
                try
                {
                    return std::stol(val);
                }
                catch (...) {}
            }
            return defaultValue;
        }

        void SetValue(const std::string& section, const std::string& key, const std::string& value)
        {
            m_Sections[section][key] = value;
        }

        void SetLongValue(const std::string& section, const std::string& key, long value)
        {
            SetValue(section, key, std::to_string(value));
        }

    private:
        std::string m_Filename;
        std::unordered_map<std::string, Section> m_Sections;

        static std::string Trim(const std::string& s)
        {
            size_t start = s.find_first_not_of(" \t\r\n");
            size_t end = s.find_last_not_of(" \t\r\n");
            return start == std::string::npos ? "" : s.substr(start, end - start + 1);
        }
    };
}