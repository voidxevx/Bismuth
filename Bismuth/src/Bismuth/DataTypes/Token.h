#pragma once

#include "../Core.h"

#include <optional>
#include <string>

namespace bismuth
{

	enum BISMUTH_API tokenType
	{
		Identifier,
		Assignment, // =
		LineEnd, // ;
		ExpressionStart, // (
		ExpressionEnd, // )
		StaticInt, 
	};

	struct BISMUTH_API token
	{
		tokenType Type;
		std::optional<std::string> Value;

		token(tokenType type)
			: Type(type)
		{}
		token(tokenType type, std::string val)
			: Type(type)
			, Value(val)
		{}
	};

}