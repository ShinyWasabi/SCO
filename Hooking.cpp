#include "Hooking.hpp"

CallHookMemory::CallHookMemory()
{
	m_Memory = VirtualAlloc((PVOID)((std::uintptr_t)GetModuleHandle(0) + 0x40000000), 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	m_Offset = 0;
}

PVOID CallHookMemory::AllocateJumpSequenceImpl(PVOID func)
{
	m_Offset = m_Offset + ((16 - (m_Offset % 16)) % 16);

	*m_Memory.Add(m_Offset).As<std::int16_t*>() = 0xB848;
	*m_Memory.Add(m_Offset).Add(2).As<PVOID*>() = func;
	*m_Memory.Add(m_Offset).Add(10).As<std::int16_t*>() = 0xE0FF;

	m_Offset += 12;

	return m_Memory.Add(m_Offset).Sub(12).As<PVOID>();
}

CallSiteHook::Hook::Hook(PVOID location, PVOID hook) :
	m_Location(location),
	m_Hook(hook),
	m_Enabled(false)
{
	auto seq = CallHookMemory::AllocateJumpSequence(hook);
	m_PatchedBytes[0] = 0xE8;
	*(std::int32_t*)&m_PatchedBytes[1] = (std::int32_t)((std::uint64_t)seq - (std::uint64_t)location - 5);
	memcpy(m_OriginalBytes, location, 5);
	m_OriginalFunction = Memory(location).Add(1).Rip().As<PVOID>();
}

void CallSiteHook::Hook::Enable()
{
	if (!m_Enabled)
	{
		DWORD oldProtect, temp;
		VirtualProtect(m_Location, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(m_Location, m_PatchedBytes, 5);
		VirtualProtect(m_Location, 5, oldProtect, &temp);
		m_Enabled = true;
	}
}

void CallSiteHook::Hook::Disable()
{
	if (m_Enabled)
	{
		DWORD oldProtect, temp;
		VirtualProtect(m_Location, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(m_Location, m_OriginalBytes, 5);
		VirtualProtect(m_Location, 5, oldProtect, &temp);
		m_Enabled = false;
	}
}

std::shared_ptr<CallSiteHook::Hook> CallSiteHook::AddHookImpl(PVOID location, PVOID hook)
{
	auto hookPtr = std::make_shared<Hook>(location, hook);

	m_Hooks.push_back(hookPtr);
	return hookPtr;
}