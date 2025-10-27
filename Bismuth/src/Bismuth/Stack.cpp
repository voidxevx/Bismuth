#include "Stack.h"

namespace bis::util
{
	template<typename _T> BISMUTH_API
	stack<_T>::~stack()
	{
		if (m_stack_top)
		{
			m_stack_top->__purge();
			delete m_stack_top;
			m_stack_top = nullptr;
		}
	}

	template<typename _T> BISMUTH_API void 
	stack<_T>::push(_T value)
	{
		stack_item<_T>* newItem = new stack_item<_T>(value, m_stack_top);
		++m_stack_size;
		m_stack_top = newItem;
	}

	template<typename _T> BISMUTH_API _T 
	stack<_T>::pop()
	{
		_T topValue = m_stack_top->item;
		stack_item<_T>* nextItem = m_stack_top->next_item;
		delete m_stack_top;
		m_stack_top = nextItem;
		--m_stack_size;
		return topValue;
	}

	template<typename _T> BISMUTH_API _T 
	stack<_T>::at(const size_t i)
	{
		if (m_stack_top)
			return m_stack_top->__get_item(i);
		return {};
	}

}