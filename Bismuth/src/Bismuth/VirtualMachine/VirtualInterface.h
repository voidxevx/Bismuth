#pragma once

#include "../Core.h"

#include "variable.h"

namespace bis
{

	struct Register
	{
		//union
		//{
		//	long X;
		//	//int 
		//};
	};

	constexpr size_t STACK_MAX_SIZE = 128;

	class VertualInterface
	{
	public:
		VertualInterface();
		~VertualInterface();

		void PushToStack(__IVariable*);
		__IVariable* PopFromStack(__IVariable*);

	private:
		__IVariable** m_Stack;
	};

}