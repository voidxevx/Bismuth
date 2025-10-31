#pragma once

#include "Core.h"

#include "variable.h"

#include <map>
#include <optional>

namespace bis
{

	union PropertyAccess
	{
		enum Domain : uint8_t
		{
			Public,
			Protected,
			Private,
		};
		enum Point : uint8_t
		{
			Indirect,
			Inherited,
			Direct,
		};
	};

	extern BISMUTH_API bool CompareAccess(PropertyAccess::Domain, PropertyAccess::Point);

	struct PropertyTemplate
	{
		ValueType::ValueType Type;
		size_t ID;
		PropertyAccess::Domain Domain;

		PropertyTemplate()
			: Type(0)
			, ID(0)
			, Domain(PropertyAccess::Public)
		{}

		PropertyTemplate(ValueType::ValueType type, size_t id, PropertyAccess::Domain domain)
			: Type(type)
			, ID(id)
			, Domain(domain)
		{}
	};

	struct PropertyLocation
	{
		ValueType::ValueType Type;
		unsigned int Offset;
		PropertyAccess::Domain Domain;

		PropertyLocation()
			: Type(0)
			, Offset(0)
			, Domain(PropertyAccess::Public)
		{}

		PropertyLocation(ValueType::ValueType type, unsigned int offset, PropertyAccess::Domain domain)
			: Type(type)
			, Offset(offset)
			, Domain(domain)
		{}
	};

	class BISMUTH_API ClassTemplate
	{
	public:
		ClassTemplate(std::vector<PropertyTemplate>, std::shared_ptr<ClassTemplate>);

		std::optional<PropertyLocation> GetStaticProperty(const size_t, PropertyAccess::Point) const;
		const unsigned int GetPropertyCount() const;
		const std::map<size_t, PropertyLocation>& GetProperties() const { return m_Properties; }

	private:
		std::map<size_t, PropertyLocation> m_Properties;
		std::shared_ptr<ClassTemplate> m_Parent;
	};

	class BISMUTH_API __ClassVariable : public __IVariable
	{
	public:
		__ClassVariable(std::shared_ptr<ClassTemplate> _template)
			: __IVariable(ValueType::__ValueType(ValueType::typeHint::Class, ValueType::typeModifier::Pointer))
			, m_Template(_template)
		{}

		inline const std::shared_ptr<ClassTemplate> GetTemplate() const { return m_Template; }

	private:
		const std::shared_ptr<ClassTemplate> m_Template;
	};

	typedef __ClassVariable bisClass;

}