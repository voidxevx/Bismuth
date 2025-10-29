#pragma once
#include "../Node.h"

#include "../../Core.h"

namespace bis::nodes
{

	class BISMUTH_API Node_And : public Node
	{
	public:
		Node_And(Node* next, Node* last)
			: Node(NodeInstruction::And, NodeClass::LogicOperations, next, last)
		{}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_Or : public Node
	{
	public:
		Node_Or(Node* next, Node* last)
			: Node(NodeInstruction::Or, NodeClass::LogicOperations, next, last)
		{
		}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_Xor : public Node
	{
	public:
		Node_Xor(Node* next, Node* last)
			: Node(NodeInstruction::Xor, NodeClass::LogicOperations, next, last)
		{}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_Not : public Node
	{
	public:
		Node_Not(Node* next, Node* last)
			: Node(NodeInstruction::Not, NodeClass::LogicOperations, next, last)
		{}

		void Evaluate() override final;
	};

	class BISMUTH_API Node_Compare : public Node
	{
	public:
		Node_Compare(Node* next, Node* last)
			: Node(NodeInstruction::Compare, NodeClass::LogicOperations, next, last)
		{}

		void Evaluate() override final;
	};

}