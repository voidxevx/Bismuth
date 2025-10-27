#include "scope.h"

namespace bis
{

	Scope::Scope(Scope* parent, __ClassVariable* root)
		: m_ParentScope(parent)
		, m_Root(root)
	{}

	void
	Scope::InsertVariable(size_t id, const unsigned int location)
	{
		m_Variables[id] = location;
		++m_ScopeSize;
	}

	std::optional<const unsigned int >
	Scope::GetVariableLocation(size_t id) const
	{
		if (m_Variables.count(id) > 0)
			return m_Variables.at(id);
		else if (m_ParentScope)
			return m_ParentScope->GetVariableLocation(id);
		else
			return std::nullopt;
	}

}