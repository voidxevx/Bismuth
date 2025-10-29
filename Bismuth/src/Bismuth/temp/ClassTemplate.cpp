#include "ClassTemplate.h"

namespace bis
{

	inline BISMUTH_API bool
	CompareAccess(PropertyAccess::Domain domain, PropertyAccess::Point point)
	{
		return domain == PropertyAccess::Public || point == PropertyAccess::Direct || (point == domain /* if inherited and protected */);
	}

	ClassTemplate::ClassTemplate(std::vector<PropertyTemplate> properties, std::shared_ptr<ClassTemplate> parent)
		: m_Parent(parent)
	{
		unsigned int offset = (m_Parent ? m_Parent->GetPropertyCount() : 1);
		for (const auto& prop : properties)
			m_Properties[prop.ID] = { prop.Type, offset, prop.Domain };
	}

	std::optional<PropertyLocation>
	ClassTemplate::GetStaticProperty(size_t id, PropertyAccess::Point accessPoint) const
	{
		if (m_Properties.count(id) > 0 && CompareAccess(m_Properties.at(id).Domain, accessPoint))
			return m_Properties.at(id);
		else if (m_Parent)
			return GetStaticProperty(id, (accessPoint == PropertyAccess::Direct ? PropertyAccess::Inherited : accessPoint));
		else
			return std::nullopt;
	}

	const unsigned int 
	ClassTemplate::GetPropertyCount() const
	{
		if (m_Parent)
			return m_Properties.size() + m_Parent->GetPropertyCount();
		else
			return m_Properties.size();
	}

}