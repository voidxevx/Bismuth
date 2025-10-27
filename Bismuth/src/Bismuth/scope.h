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
		Scope(Scope*, __ClassVariable* = nullptr);

		inline const size_t GetScopeSize() const { return m_ScopeSize; }
		inline Scope* GetParentScope() const { return m_ParentScope; }
		inline __ClassVariable* GetRoot() const { return m_Root; }

		void InsertVariable(size_t, const unsigned int);

		std::optional<const unsigned int> GetVariableLocation(size_t) const;

	private:
		Scope* m_ParentScope;
		std::map<size_t, unsigned int> m_Variables;
		__ClassVariable* m_Root;
		size_t m_ScopeSize = 0;
	};

}