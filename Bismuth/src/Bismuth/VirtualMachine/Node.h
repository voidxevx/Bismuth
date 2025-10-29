#pragma once

#include "../Core.h"

#include "variable.h"

#include <stdint.h>

namespace bis::nodes
{

	enum NodeInstruction : uint8_t
	{
		// Stack operations
		StackPush,
		StackPop,
		Move,

		// arithmatic
		Add,
		Sub,
		Mul,
		Div,
		Inc,
		Dec,

		// logic operations
		And,
		Or,
		Xor,
		Not,
		Compare,

		// flow control
		Jump,
		Call,
		Return,

	};

	enum class NodeClass : uint8_t
	{
		StackOperation,
		Arithmatic,
		LogicOperations,
		FlowControl,
	};

	class BISMUTH_API Node
	{
	public:
		~Node() = default;
		
		const NodeInstruction GetInstruction() const { return m_Instruction; }
		const NodeClass GetClass() const { return m_ClassType; }

		void SetRightOperand(__IVariable* right) { m_RightOperand = right; }
		void SetLeftOperand(__IVariable* left) { m_LeftOperand = left; }

		virtual void Evaluate() {}; // Each nodes operation emplemented here

	protected:
		Node(NodeInstruction instr, NodeClass _class, Node* next = nullptr, Node* last = nullptr)
			: m_Instruction(instr)
			, m_ClassType(_class)
			, m_Next(next)
			, m_Last(last)
		{}

		Node* m_Next, *m_Last;
		__IVariable *m_RightOperand = nullptr, *m_LeftOperand = nullptr;

	private:
		const NodeInstruction m_Instruction;
		const NodeClass m_ClassType;
	};

}