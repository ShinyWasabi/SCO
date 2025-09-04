#include "ScriptFunction.hpp"
#include "Pointers.hpp"
#include "rage/scrThread.hpp"
#include "rage/scrProgram.hpp"
#include "rage/tlsContext.hpp"

namespace SCOL
{
	void ScriptFunction::CallImpl(const joaat_t script, const joaat_t pc, const std::vector<std::uint64_t>& args, void* returnValue, std::uint32_t returnSize)
	{
		auto thread = rage::scrThread::FindScriptThread(script);
		auto program = rage::scrProgram::FindScriptProgram(script);

		if (!thread || !program || !pc)
			return;

		if (auto byte = program->GetCode(pc); !byte || *byte != 0x2D) // This will fail if the PC ends up pointing to some opcode's operand whose value is 0x2D, but whatever.
		{
			LOGF(WARNING, "Attempted to execute a script function from an invalid address.");
			return;
		}

		auto tlsCtx = rage::tlsContext::Get();
		auto stack = thread->m_Stack;
		auto ogThread = tlsCtx->m_CurrentScriptThread;

		tlsCtx->m_CurrentScriptThread = thread;
		tlsCtx->m_ScriptThreadActive = true;

		auto ctx = thread->m_Context;
		auto topStack = ctx.m_StackPointer;

		for (auto& arg : args)
			stack[ctx.m_StackPointer++].Any = arg;

		stack[ctx.m_StackPointer++].Any = 0;
		ctx.m_ProgramCounter = pc;
		ctx.m_State = rage::scrThreadState::RUNNING;

		g_Pointers.RunScriptThread(stack, g_Pointers.ScriptGlobals, program, &ctx);

		tlsCtx->m_CurrentScriptThread = ogThread;
		tlsCtx->m_ScriptThreadActive = ogThread != nullptr;

		if (returnValue)
			std::memcpy(returnValue, stack + topStack, returnSize);
	}
}