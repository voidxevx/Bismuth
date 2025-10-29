#pragma once
#include "../Node.h"

#include "../../Core.h"

namespace bis::nodes
{

	template<typename _T>
	class BISMUTH_API Node_Add : public Node
	{
	public:
		Node_Add(Node* next, Node* last)
			: Node(NodeInstruction::Add, NodeClass::Arithmatic, next, last)
		{}

		void Evaluate() override final;
	};

	template<typename _T>
	class BISMUTH_API Node_Sub : public Node
	{
	public:
		Node_Sub(Node* next, Node* last)
			: Node(NodeInstruction::Sub, NodeClass::Arithmatic, next, last)
		{}

		void Evaluate() override final;
	};

	template<typename _T>
	class BISMUTH_API Node_Mul : public Node
	{
	public:
		Node_Mul(Node* next, Node* last)
			: Node(NodeInstruction::Mul, NodeClass::Arithmatic, next, last)
		{}

		void Evaluate() override final;
	};

	template<typename _T>
	class BISMUTH_API Node_Div : public Node
	{
	public:
		Node_Div(Node* next, Node* last)
			: Node(NodeInstruction::Div, NodeClass::Arithmatic, next, last)
		{}

		void Evaluate() override final;
	};

	template<typename _T>
	class BISMUTH_API Node_Inc : public Node
	{
	public:
		Node_Inc(Node* next, Node* last)
			: Node(NodeInstruction::Inc, NodeClass::Arithmatic, next, last)
		{}

		void Evaluate() override final;
	};

	template<typename _T>
	class BISMUTH_API Node_Dec : public Node
	{
	public:
		Node_Dec(Node* next, Node* last)
			: Node(NodeInstruction::Dec, NodeClass::Arithmatic, next, last)
		{ }

		void Evauluate() override final;
	};

}