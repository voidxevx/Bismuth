#pragma once

#include "Core.h"

#include "variable.h"
#include "ClassTemplate.h"

#include <map>

namespace bis
{

	class BISMUTH_API Scope
	{
	public:
		Scope(Scope*, const unsigned int = 0);

		inline const size_t GetScopeSize() const { return m_ScopeSize; }
		inline Scope* GetParentScope() const { return m_ParentScope; }
		inline const unsigned int GetRoot() const { return m_Root; }

		void InsertVariable(size_t);
		void InsertProperty();

		std::optional<const unsigned int> GetVariableLocation(size_t, size_t = 0) const;

	private:
		Scope* m_ParentScope;
		std::map<size_t, unsigned int> m_Variables;
		const unsigned int m_Root;
		size_t m_ScopeSize = 0;
	};

}