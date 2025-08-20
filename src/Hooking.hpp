#pragma once

namespace SCOL
{
    class Hooking
    {
    public:
        struct Hook
        {
            std::string m_Name;
            void* m_Target;
            void* m_Detour;
            void* m_Original = nullptr;
        };

        static bool Init();

        template<typename T>
        static T GetOriginal(const std::string& name)
        {
            for (auto& hook : m_Hooks)
            {
                if (hook.m_Name == name)
                    return reinterpret_cast<T>(hook.m_Original);
            }

            return nullptr;
        }

    private:
        template<typename T, typename D>
        static void AddHook(const std::string& name, T target, D detour)
        {
            m_Hooks.push_back({ name, reinterpret_cast<void*>(target), reinterpret_cast<void*>(detour) });
        }

        static inline std::vector<Hook> m_Hooks;
    };
}