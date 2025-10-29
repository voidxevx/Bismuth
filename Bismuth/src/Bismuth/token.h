#pragma once

#include "Core.h"
#include <cstdint>
#include <optional>

namespace bis
{

	enum BISMUTH_API tokenType : uint8_t
	{
		Start, // the start of a token string
		End, // the end of a token string
		ExpressionStart, // (
		ExpressionEnd, // )
		ExpressionBreak, // ,
		ScopeStart, // {
		ScopeEnd, // }
		LineEnd, // ; or \n

		Identifier, // anything not a keyword
		Property, // :
		Assignement, // =

		MutableType, // mutable
		ConstType, // const
		PublicDomain, // public
		ProtectedDomain, // protected
		PrivateDomain, // private
		ClassDecl, // class
		Inheritance, // extends

		StaticInt, // 0
		StaticLong, // 0l
		StaticFloat, // 0.0f
		StaticDouble, // 0.00d
		StaticByte, // '' or 0b
		StaticBoolTrue, // true
		StaticBoolFalse, // false
		StaticString, // ""
		StaticNull, // null
	};

	struct BISMUTH_API token
	{
		tokenType Type;
		std::optional<size_t> ID;
		token* Next;
		token* Last;

		token(token* next, token* last, tokenType type)
			: Type(type)
			, ID(std::nullopt)
			, Next(next)
			, Last(last)
		{}

		token(token* next, token* last, tokenType type, size_t id)
			: Type(type)
			, ID(id)
			, Next(next)
			, Last(last)
		{}

		token* __GetLast()
		{
			if (Last)
				return Last->__GetLast();
			else
				return this;
		}

		void append(token* next)
		{
			this->Next = next->__GetLast();
		}

		void __Purge()
		{
			__PurgeNext();
			__PurgeLast();

			delete this;
		}

		void __PurgeNext()
		{
			if (Next)
				Next->__PurgeNext();

			delete Next;
			Next = nullptr;
		}

		void __PurgeLast()
		{
			if (Last)
				Last->__PurgeLast();

			delete Last;
			Last = nullptr;
		}
	};

}