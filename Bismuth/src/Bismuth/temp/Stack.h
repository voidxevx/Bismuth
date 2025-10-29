#pragma once

#include "Core.h"

#include "variable.h"

namespace bis::util
{

	template<typename _T>
	struct BISMUTH_API stack_item
	{
		_T item;
		stack_item<_T>* next_item;

		stack_item(_T value, stack_item<_T>* next)
			: item(value)
			, next_item(next)
		{}

		_T __get_item(const size_t i)
		{
			if (i == 0)
				return item;
			else
				return next_item->__get_item(i - 1);
		}

		void __purge()
		{
			if (next_item)
			{
				next_item->__purge();
				delete next_item;
			}
		}
	};


	template<typename _T>
	class BISMUTH_API stack
	{
	public:
		explicit stack() = default;
		~stack();

		void push(_T);
		_T pop();

		_T at(const size_t);
		inline _T operator[](const size_t i) { return this->at(i); }

		inline const size_t size() const { return m_stack_size; }

	private:
		stack_item<_T>* m_stack_top = nullptr;
		size_t m_stack_size = 0;
	};

	template class BISMUTH_API stack<__IVariable*>;

}