#include "state.h"

namespace bis
{

	state::state()
	{
		//m_VariableStack = (__IVariable**)malloc(MAX_STACK_SIZE * sizeof(__IVariable*));
	}

	state::~state()
	{
		//free(m_VariableStack);
	}

	void
	state::PushVariable(size_t id, __IVariable* var)
	{
		//m_VariableStack[m_VariableStackTop] = var;
		//m_CurrentScope->InsertVariable(id, m_VariableStackTop);
		//++m_VariableStackTop;

		m_CurrentScope->InsertVariable(id, m_VarStack.size());
		m_VarStack.push(var);
	}

	std::optional<__IVariable*> state::GetRawVariable(size_t id)
	{
		std::optional<const unsigned int> loc = m_CurrentScope->GetVariableLocation(id);
		if (loc.has_value())
			//return m_VariableStack[loc.value()];
			return m_VarStack[m_VarStack.size() - (loc.value() + 1)];
		else
			return std::nullopt;
	}

	template<typename _T> std::optional<_T>
	state::GetVariable(size_t id)
	{
		std::optional<__IVariable*> var_opt = GetRawVariable(id);
		if (var_opt.has_value())
			return __SmartCast<_T>(var_opt.value());
		else
			return std::nullopt;
	}


	void
	state::PushScope(__ClassVariable* root)
	{
		Scope* newScope = new Scope(m_CurrentScope, root);
		m_CurrentScope = newScope;
	}

	void
	state::PopScope()
	{
		const size_t deallocAmount = m_CurrentScope->GetScopeSize();
		for (unsigned int i{}; i < deallocAmount; ++i)
		{
			//delete m_VariableStack[m_VariableStackTop - 1];
			//m_VariableStack[m_VariableStackTop - 1] = nullptr;
			//--m_VariableStackTop;
			m_VarStack.pop();
		}
		Scope* nextScope = m_CurrentScope->GetParentScope();
		delete m_CurrentScope;
		m_CurrentScope = nextScope;
	}

}