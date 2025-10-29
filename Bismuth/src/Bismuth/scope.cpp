#include "scope.h"

namespace bis
{

	Scope::Scope(Scope* parent, const unsigned int root)
		: m_ParentScope(parent)
		, m_Root(root)
	{}

	void
	Scope::InsertVariable(size_t id)
	{
		m_Variables[id] = (unsigned int)m_ScopeSize;
		++m_ScopeSize;
	}

	void
	Scope::InsertProperty()
	{
		++m_ScopeSize;
	}

	std::optional<const unsigned int >
	Scope::GetVariableLocation(size_t id, size_t offset) const
	{
		if (m_Variables.count(id) > 0)
			return (m_ScopeSize - m_Variables.at(id) - 1) + offset;
		else if (m_ParentScope)
			return m_ParentScope->GetVariableLocation(id, offset + m_ScopeSize);
		else
			return std::nullopt;
	}

}