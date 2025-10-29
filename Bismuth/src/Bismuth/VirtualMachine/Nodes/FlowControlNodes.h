#pragma once
#include "../Node.h"

#include "../../Core.h"

namespace bis::nodes
{

	enum ComparisonFlags : uint8_t
	{
		NONE = 0,
		Equal = 0b1,
		Greater = 0b10,
		Less = 0b100,
		Not = 0b1000,
	};

	class BISMUTH_API Node_Jump : public Node
	{
	public:
		Node_Jump(Node* next, Node* last, ComparisonFlags flags)
			: Node(NodeInstruction::Jump, NodeClass::FlowControl, next, last)
			, m_Flags(flags)
		{}

		void Evaluate() override final;
	private:
		ComparisonFlags m_Flags;
	};

	class BISMUTH_API Node_Call : public Node
	{
	public:
		Node_Call(Node* next, Node* last)
			: Node(NodeInstruction::Call, NodeClass::FlowControl, next, last)
		{}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_Return : public Node
	{
	public:
		Node_Return(Node* next, Node* last)
			: Node(NodeInstruction::Return, NodeClass::FlowControl, next, last)
		{}

		void Evaluate() override final;
	};



}