#pragma once

#include "Core.h"

#include "variable.h"
#include "ClassTemplate.h"
#include "scope.h"
#include "Stack.h"
#include "token.h"

namespace bis
{
	constexpr size_t MAX_STACK_SIZE = 128;

	class BISMUTH_API state
	{
	public:
		state();
		~state();

		void PushVariable(size_t, __IVariable*, bool);
		std::optional<__IVariable*> GetRawVariable(size_t);
		template<typename _T> std::optional<_T> GetVariable(size_t);

		void PushScope(const unsigned int = 0);
		void PopScope();

		void PushTokens(token* const);
		void popToken();

		const unsigned int TraceIdentifier();
		token* tokenize(const std::string&);

	private:
		util::stack<__IVariable*> m_VarStack;
		util::stack<__IVariable*> m_ReturnStack;
		std::map < size_t, std::string> m_Constants;
		Scope* m_CurrentScope = nullptr;
		token* m_TopToken = nullptr;
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