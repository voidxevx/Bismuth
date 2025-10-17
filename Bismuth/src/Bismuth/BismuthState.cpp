#include "BismuthState.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cctype>
#include <cstring>
#include <algorithm>

namespace bismuth
{

	state::state()
	{
		m_ReturnStack = (const void**)malloc(STACK_MAX_SIZE * sizeof(const void*));
		m_ReturnDeallocations = (bool*)malloc(STACK_MAX_SIZE * sizeof(bool));
	}

	state::~state()
	{
		free(m_ReturnStack);
		free(m_ReturnDeallocations);
	}


	template<typename _T>
	void state::PushVariable(const std::string& name, _T val)
	{
		m_Variables[name] = new BismuthVariable<_T>(name, val);
	}

	template<typename _T>
	BismuthVariable<_T>& state::GetVariable(const std::string& name)
	{
		return *dynamic_cast<BismuthVariable<_T>*>(m_Variables[name]);
	}
	




	std::vector<token> state::tokenize(const std::string& source)
	{
		std::stringstream buffer(source);
		std::vector<token> tokens;

		std::string c_token;
		std::vector<std::string> c_InternTokens;
		while (std::getline(buffer, c_token, ' '))
		{
			c_InternTokens.clear();
			BreakToken(c_token, c_InternTokens);
			for (unsigned int i{}; i < c_InternTokens.size(); ++i)
			{
				if (c_InternTokens[i] == "=")
					tokens.push_back(token(tokenType::Assignment));
				else if (c_InternTokens[i] == ";")
					tokens.push_back(token(tokenType::LineEnd));
				else if (c_InternTokens[i] == "(")
					tokens.push_back(token(tokenType::ExpressionStart));
				else if (c_InternTokens[i] == ")")
					tokens.push_back(token(tokenType::ExpressionEnd));
				else if (is_num(c_InternTokens[i]))
					tokens.push_back(token(tokenType::StaticInt, c_InternTokens[i]));
				else
					tokens.push_back(token(tokenType::Identifier, c_InternTokens[i]));
			}
		}

		return tokens;
	}


	void state::BreakToken(const std::string& str, std::vector<std::string>& tokens) const
	{
		std::stringstream buffer;
		for (unsigned int i{}; i < str.length(); ++i)
		{
			if (!std::isspace(str[i]))
			{
				if (!std::isalnum(str[i]))
				{
					if (buffer.str() != "")
						tokens.push_back(buffer.str());
					tokens.push_back(std::string(1, str[i]));
					buffer.str("");
				}
				else
					buffer << str[i];
			}
		}

		if (buffer.str() != "")
			tokens.push_back(buffer.str());
	}





	bool state::is_num(const std::string& str) const
	{
		try
		{
			int num = std::stoi(str);
		}
		catch (const std::invalid_argument)
		{
			return false;
		}
		catch (const std::out_of_range)
		{
			return false;
		}

		return true;
	}


	void state::pushToReturn(const void* const val, bool dealloc)
	{
		if (m_ReturnsStackTop < STACK_MAX_SIZE)
		{
			m_ReturnStack[m_ReturnsStackTop] = val;
			m_ReturnDeallocations[m_ReturnsStackTop] = dealloc;
			++m_ReturnsStackTop;
		}
	}

	template<typename _T>
	_T const state::popFromReturn()
	{
		const void* const val = m_ReturnStack[m_ReturnsStackTop - 1];
		_T copy_val = *static_cast<const _T*>(val);
		if (m_ReturnDeallocations[m_ReturnsStackTop - 1])
			delete val;
		--m_ReturnsStackTop;
		return copy_val;
	}

	void state::EvaluateExpression(const std::vector<token>& tokens, unsigned int& i)
	{
		while (tokens[(++i)].Type != tokenType::LineEnd && tokens[i].Type != tokenType::ExpressionEnd)
		{
			token c_token = tokens[i];

			/*
			*	Static: return value
			*	Identifier: function or variable TODO: add identifier path for classes and objects
			*/
			if (c_token.Type == tokenType::StaticInt)
			{
				pushToReturn(new int(std::stoi(c_token.Value.value())), true);
			}
			else if (c_token.Type == tokenType::Identifier)
			{
				const std::string identifierName = c_token.Value.value();
				
				if (tokens[i + 1].Type == tokenType::ExpressionStart) // call function + push return
				{

				}
				else if (m_Variables.count(identifierName) > 0)// push variable value to stack	
				{
					IBismuthVariable* var = m_Variables[identifierName];
					const std::string staticType = m_Variables[identifierName]->GetStaticType();

					if (staticType == "int")
					{
						bis_int* var_int = dynamic_cast<bis_int*>(var);
						pushToReturn(&var_int->Get());
					}
					else if (staticType == "str")
					{
						bis_string* var_str = dynamic_cast<bis_string*>(var);
						pushToReturn(&var_str->Get());
					}
					else if (staticType == "float")
					{
						bis_float* var_float = dynamic_cast<bis_float*>(var);
						pushToReturn(&var_float->Get());
					}
					else if (staticType == "uint")
					{
						bis_uint* var_uint = dynamic_cast<bis_uint*>(var);
						pushToReturn(&var_uint->Get());
					}
					else if (staticType == "voidptr")
					{
						bis_voidptr* var_voidptr = dynamic_cast<bis_voidptr*>(var);
						pushToReturn(&var_voidptr->Get());
					}
				}
			}
		}
	}


	void state::Evaluate(const std::vector<token>& tokens)
	{
		unsigned int i{};
		while (i < tokens.size())
		{
			token c_token = tokens[i];
			/* Identifier - 
			*	followed by another identifier: variable instantiation
			*	followed by assignment: change variable value
			*	followed by expression: function call
			*/
			if (c_token.Type == tokenType::Identifier)
			{

				if (tokens[i + 1].Type == tokenType::Identifier) // var instantiation
				{
					++i; // to name identifier
					const std::string varType = c_token.Value.value();
					c_token = tokens[i]; // identifier/name

					IBismuthVariable* var{};

					if (varType == "int")
						var = new bis_int(c_token.Value.value(), 0);
					else if (varType == "str")
						var = new bis_string(c_token.Value.value(), "");
					else if (varType == "float")
						var = new bis_float(c_token.Value.value(), 0.0f);
					else if (varType == "uint")
						var = new bis_uint(c_token.Value.value(), 0U);
					else if (varType == "voidptr")
						var = new bis_voidptr(c_token.Value.value(), nullptr);

					/* Variable Instantiation -
					*	followed by assignment: instantiate with value
					*	followed by end line: instantiated as null
					*/
					if (tokens[i + 1].Type == tokenType::Assignment) // instantiate with value
					{
						++i; // to expression
						EvaluateExpression(tokens, i);
						if (varType == "int")
						{
							bis_int* var_int = dynamic_cast<bis_int*>(var);
							var_int->Set(popFromReturn<int>());
						} 
						else if (varType == "string")
						{
							bis_string* var_str = dynamic_cast<bis_string*>(var);
							var_str->Set(popFromReturn<std::string>());
						}
						else if (varType == "float")
						{
							bis_float* var_float = dynamic_cast<bis_float*>(var);
							var_float->Set(popFromReturn<float>());
						}
						else if (varType == "uint")
						{
							bis_uint* var_uint = dynamic_cast<bis_uint*>(var);
							var_uint->Set(popFromReturn<unsigned int>());
						}
						else if (varType == "voidptr")
						{
							bis_voidptr* var_voidptr = dynamic_cast<bis_voidptr*>(var);
							var_voidptr->Set(popFromReturn<const void*>());
						}


						// TODO: Expression evaluation + return stack

					} // else instantiate null



					m_Variables[c_token.Value.value()] = std::move(var);


				}
				else if (tokens[i + 1].Type == tokenType::Assignment) // var change
				{
					//++i; // to variable assignment
					const std::string varName = c_token.Value.value();
					IBismuthVariable* var = nullptr;
					if (m_Variables.count(varName) > 0)
						var = m_Variables[varName];
					else
						; // TODO: Push errors

					if (var)
					{
						const std::string varType = var->GetStaticType();

						EvaluateExpression(tokens, i);
						if (varType == "int")
						{
							bis_int* var_int = dynamic_cast<bis_int*>(var);
							var_int->Set(popFromReturn<int>());
						}
						else if (varType == "str")
						{
							bis_string* var_string = dynamic_cast<bis_string*>(var);
							var_string->Set(popFromReturn<std::string>());
						}
						else if (varType == "float")
						{
							bis_float* var_float = dynamic_cast<bis_float*>(var);
							var_float->Set(popFromReturn<float>());
						}
						else if (varType == "uint")
						{
							bis_uint* var_uint = dynamic_cast<bis_uint*>(var);
							var_uint->Set(popFromReturn<unsigned int>());
						}
						else if (varType == "voidptr")
						{
							bis_voidptr* var_voidptr = dynamic_cast<bis_voidptr*>(var);
							var_voidptr->Set(popFromReturn<const void*>());
						}
					}
					
				}
				else if (tokens[i + 1].Type == tokenType::ExpressionStart) // function call
				{
					const std::string identifierName = c_token.Value.value();

					EvaluateExpression(tokens, i);

					if (identifierName == "print_str")
					{
						const std::string val = popFromReturn<std::string>();
						std::cout << val << "\n";
					}
					else; // TODO: custom functions
				}

			}

			++i;
		}
	}


	void state::DoString(const std::string& source)
	{
		m_OperationThread = std::thread([this](const std::string& src){
			Evaluate(tokenize(src));
		}, source);
	}







	BismuthClassTemplate::BismuthClassTemplate(const std::string& className, const std::vector<std::pair<std::string, ValueType>>& properties, const std::shared_ptr<BismuthClassTemplate> parent)
		: m_Name(className)
		, m_ParentClass(parent)
		, m_TotalProperties(properties.size())
	{
		unsigned int offset = 0;
		for (const auto& prop : properties)
		{
			m_Properties[prop.first] = { prop.second, offset };
		}
	}

	const std::optional<BismuthClassProperty> BismuthClassTemplate::getStaticProperty(const std::string& name) const
	{
		if (m_Properties.count(name) > 0)
			return m_Properties.at(name);
		else if (m_ParentClass)
			return m_ParentClass->getStaticProperty(name).value();
		else
			return std::nullopt;
	}



	BismuthClass::BismuthClass(const std::shared_ptr<BismuthClassTemplate> _template)
		: m_ClassTemplate(_template)
	{
		m_Properties = (const void**)malloc(_template->m_TotalProperties * sizeof(const void*));
	}

	template<typename _T>
	const std::optional<_T> BismuthClass::getProperty(const std::string& name) const
	{
		std::optional<BismuthClassProperty> prop = m_ClassTemplate->getStaticProperty(name);
		if (prop.has_value())
		{
			const void* val = m_Properties[prop.value().Offset];
			switch (prop.value().Type)
			{
			case ValueType::Int:
				return static_cast<_T>(static_cast<int>(val));
			case ValueType::String:
				return static_cast<_T>(static_cast<std::string>(val));
			case ValueType::Float:
				return static_cast<_T>(static_cast<float>(val));
			case ValueType::Uint:
				return static_cast<_T>(static_cast<unsigned int>(val));
			case ValueType::Analagous:
				return static_cast<_T>(val);
			default:
				return std::nullopt;
			}
		}
		else return std::nullopt;
	}

	template<typename _T>
	void BismuthClass::setProperty(const std::string& name, const _T* const newValue)
	{
		std::optional<BismuthClassProperty> prop = m_ClassTemplate->getStaticProperty(name);
		if (prop.has_value())
		{
			m_Properties[prop.value().Offset] = newValue;
		}
	}

}