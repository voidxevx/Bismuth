#pragma once
#include "../Node.h"

#include "../../Core.h"

namespace bis::nodes
{

	class BISMUTH_API Node_StackPush : public Node
	{
	public:
		Node_StackPush(Node* next, Node* last)
			: Node(NodeInstruction::StackPush, NodeClass::StackOperation, next, last)
		{}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_StackPop : public Node
	{
	public:
		Node_StackPop(Node* next, Node* last)
			: Node(NodeInstruction::StackPop, NodeClass::StackOperation, next, last)
		{}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_Move : public Node
	{
	public:
		Node_Move(Node* next, Node* last)
			: Node(NodeInstruction::Move, NodeClass::StackOperation, next, last)
		{}

		void Evaluate() override final;
	};

}