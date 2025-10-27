#pragma once

#include "Core.h"

#include "variable.h"
#include "ClassTemplate.h"
#include "scope.h"

namespace bis
{
	constexpr size_t MAX_STACK_SIZE = 128;

	class BISMUTH_API state
	{
	public:
		state();
		~state();

		void PushVariable(size_t, __IVariable*);
		std::optional<__IVariable*> GetRawVariable(size_t);
		template<typename _T> std::optional<_T> GetVariable(size_t);

		void PushScope(__ClassVariable* = nullptr);
		void PopScope();

	private:
		__IVariable** m_VariableStack;
		size_t m_VariableStackTop = 0;
		Scope* m_CurrentScope;
	};

	template BISMUTH_API std::optional<int> state::GetVariable(size_t);
	template BISMUTH_API std::optional<unsigned int> state::GetVariable(size_t);
	template BISMUTH_API std::optional<long> state::GetVariable(size_t);
	template BISMUTH_API std::optional<unsigned long> state::GetVariable(size_t);
	template BISMUTH_API std::optional<float> state::GetVariable(size_t);
	template BISMUTH_API std::optional<double> state::GetVariable(size_t);
	template BISMUTH_API std::optional<char> state::GetVariable(size_t);
	template BISMUTH_API std::optional<unsigned char> state::GetVariable(size_t);
	template BISMUTH_API std::optional<bool> state::GetVariable(size_t);
	template BISMUTH_API std::optional<std::string> state::GetVariable(size_t);

}