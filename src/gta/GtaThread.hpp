#pragma once
#include "rage/scrThread.hpp"

class scriptHandler;
class scriptHandlerNetComponent;

class GtaThread : public rage::scrThread
{
	scriptHandler* m_Handler;
	scriptHandlerNetComponent* m_NetComponent;
	std::uint32_t m_ScriptHash;
	std::uint32_t m_ForceCleanupPc;
	std::uint32_t m_ForceCleanupFp;
	std::uint32_t m_ForceCleanupSp;
	std::uint32_t m_ForceCleanupFilter;
	std::uint32_t m_ForceCleanupCause;
	std::int32_t m_InstanceId;
	char m_Padding1[0x04];
	std::uint8_t m_Flag1;
	bool m_SafeForNetworkGame;
	char m_Padding2[0x02];
	bool m_IsMinigameScript;
	char m_Padding3[0x02];
	bool m_CanBePaused;
	bool m_CanRemoveBlipsFromOtherScripts;
	char m_Padding4[0x2];
	std::uint8_t m_ForceCleanupState;
	char m_Padding5[0xC];
};
static_assert(sizeof(GtaThread) == 0x1E0);