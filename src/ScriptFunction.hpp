#pragma once

namespace SCOL
{
	class ScriptFunction
	{
		template<typename Arg>
		static void PushArg(std::uint64_t* stack, std::uint32_t& stackPtr, Arg&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<Arg>>*>(reinterpret_cast<std::uint64_t*>(stack) + (stackPtr++)) = std::forward<Arg>(value);
		}

		static void CallImpl(const joaat_t script, const joaat_t pc, const std::vector<std::uint64_t>& args, void* returnValue = 0, std::uint32_t returnSize = 0);

	public:
		template<typename Ret = void, typename... Args>
		static Ret Call(const joaat_t script, const joaat_t pc, Args... args)
		{
			std::uint32_t index{};
			std::vector<std::uint64_t> params(sizeof...(Args));
			(PushArg(params.data(), index, std::forward<Args>(args)), ...);
			if constexpr (!std::is_same_v<Ret, void>)
			{
				Ret returnValue;
				CallImpl(script, pc, params, &returnValue, sizeof(returnValue));
				return returnValue;
			}
			else
			{
				CallImpl(script, pc, params);
			}
		}
	};
}