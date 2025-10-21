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
		m_ScopeStack = (VariableScope**)malloc(STACK_MAX_SIZE * sizeof(VariableScope*));
		m_VariableStack = (IBismuthVariable**)malloc(STACK_MAX_SIZE * sizeof(IBismuthVariable*));

		m_ReturnStack = (ReturnValue*)malloc(STACK_MAX_SIZE * sizeof(ReturnValue));
	}

	state::~state()
	{
		free(m_ReturnStack);
		free(m_ScopeStack);
		free(m_VariableStack);
	}



	void state::PushScope()
	{
		m_ScopeStack[m_ScopeStackTop] = new VariableScope();
		++m_ScopeStackTop;
	}

	void state::PopScope()
	{
		unsigned int size = m_ScopeStack[m_ScopeStackTop - 1]->GetScopeSize();
		for (unsigned int i{}; i < size; ++i)
		{
			Delete_ptr(m_VariableStack[m_VariableStackTop - 1]);
			--m_VariableStackTop;
		}
		delete m_ScopeStack[m_ScopeStackTop - 1];
		--m_ScopeStackTop;
	}


	template<typename _T>
	void state::PushVariable(const std::string& name, ValueType type, _T val)
	{
		m_VariableStack[m_VariableStackTop] = new BismuthVariable<_T>(type, val);
		m_ScopeStack[m_ScopeStackTop - 1]->InsertVariable(name, m_VariableStackTop);
		++m_VariableStackTop;
	}

	void state::PushVariable(const std::string& name, IBismuthVariable* const var)
	{
		m_VariableStack[m_VariableStackTop] = var;
		m_ScopeStack[m_ScopeStackTop - 1]->InsertVariable(name, m_VariableStackTop);
		++m_VariableStackTop;
	}

	template<typename _T>
	std::optional<BismuthVariable<_T>*> state::GetVariable(const std::string& name)
	{
		for (int i = m_ScopeStackTop - 1; i >= 0; --i)
			if (m_ScopeStack[i]->HasVariable(name))
				return dynamic_cast<BismuthVariable<_T>*>(m_VariableStack[m_ScopeStack[i]->GetVariableLocation(name)]);
		return std::nullopt;
	}

	std::optional<IBismuthVariable*> state::GetVariable(const std::string& name)
	{
		for (int i = m_ScopeStackTop - 1; i >= 0; --i)
			if (m_ScopeStack[i]->HasVariable(name))
				return m_VariableStack[m_ScopeStack[i]->GetVariableLocation(name)];
		return std::nullopt;
	}
	

	void state::PushFunction(const std::string& name, IBismuthFunction* function)
	{
		m_GlobalFunctions[name] = function;
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
				else if (c_InternTokens[i] == ",")
					tokens.push_back(token(tokenType::ExpressionBreak));
				else if (c_InternTokens[i] == "(")
					tokens.push_back(token(tokenType::ExpressionStart));
				else if (c_InternTokens[i] == ")")
					tokens.push_back(token(tokenType::ExpressionEnd));
				else if (c_InternTokens[i] == "{")
					tokens.push_back(token(tokenType::ScopeStart));
				else if (c_InternTokens[i] == "}")
					tokens.push_back(token(tokenType::ScopeEnd));
				else if (c_InternTokens[i] == "nil")
					tokens.push_back(token(tokenType::StaticNil));
				else if (c_InternTokens[i] == "return")
					tokens.push_back(token(tokenType::Return));
				else if (c_InternTokens[i] == "class")
					tokens.push_back(token(tokenType::ClassDecl));
				else if (c_InternTokens[i] == "extends")
					tokens.push_back(token(tokenType::Inherite));
				else if (c_InternTokens[i] == "public")
					tokens.push_back(token(tokenType::PublicDomain));
				else if (c_InternTokens[i] == "protected")
					tokens.push_back(token(tokenType::ProtectedDomain));
				else if (c_InternTokens[i] == "private")
					tokens.push_back(token(tokenType::PrivateDomain));
				else if (c_InternTokens[i][0] == '"')
					tokens.push_back(token(tokenType::StaticString, c_InternTokens[i]));
				else if (is_num_int(c_InternTokens[i]))
					tokens.push_back(token(tokenType::StaticInt, c_InternTokens[i]));
				else if (is_num_float(c_InternTokens[i]))
					tokens.push_back(token(tokenType::StaticFloat, c_InternTokens[i]));
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
				if (str[i] == '"')
				{
					if (buffer.str() != "")
						tokens.push_back(buffer.str());
					buffer.str("");

					buffer << str[i];
					++i;
					while (str[i] != '"' && i < str.length())
					{
						if (str[i] == '%')
							buffer << ' ';
						else
							buffer << str[i];
						++i;
					}
					buffer << str[i];

					if (buffer.str() != "")
						tokens.push_back(buffer.str());
					buffer.str("");
				}
				else if (!std::isalnum(str[i]) && str[i] != '.')
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



	

	bool state::is_num_int(const std::string& str)
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
	bool state::is_num_float(const std::string& str)
	{
		try
		{
			float num = std::stof(str);
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


	void state::pushToReturn(const void* const val, ValueType type, bool dealloc)
	{
		if (m_ReturnsStackTop < STACK_MAX_SIZE)
		{
			m_ReturnStack[m_ReturnsStackTop] = { val, type, dealloc };
			//m_ReturnDeallocations[m_ReturnsStackTop] = dealloc;
			++m_ReturnsStackTop;
		}
	}




	/* Pop values from return */
	#define POPRETURN_INITIALIZE ReturnValue top = m_ReturnStack[m_ReturnsStackTop - 1];\
								 const void* const val = top.Value;
	#define POPRETURN_CLEANUP if (top.deallocate)\
									  delete val;\
							  --m_ReturnsStackTop;

	template<typename _T>
	_T state::popFromReturn()
	{
		POPRETURN_INITIALIZE

		_T copy_val = *static_cast<const _T*>(val);

		POPRETURN_CLEANUP
		return static_cast<_T>(copy_val);
	}

	template<>
	int state::popFromReturn()
	{
		POPRETURN_INITIALIZE

		int copy_val = (int)NAN;
		switch (top.Type)
		{
		case ValueType::Uint:
		case ValueType::Int:
			copy_val = *static_cast<const int*>(val);
			break;
		case ValueType::Float:
			copy_val = (int)floor(*static_cast<const float*>(val));
			break;
		case ValueType::String:
		{
			const std::string copy_val_str = *static_cast<const std::string*>(val);
			if (is_num_int(copy_val_str))
				copy_val = std::stoi(copy_val_str);
			break;
		}
		case ValueType::Analogous:
			try
			{
				copy_val = *static_cast<const int*>(val);
			}
			catch (const std::bad_cast&)
			{

			}
			break;
		default:
			break;
		}

		POPRETURN_CLEANUP
		return copy_val;
	}

	template<>
	unsigned int state::popFromReturn()
	{
		POPRETURN_INITIALIZE

		unsigned int copy_val = NAN;
		switch (top.Type)
		{
		case ValueType::Uint:
		case ValueType::Int:
			copy_val = *static_cast<const unsigned int*>(val);
			break;
		case ValueType::Float:
			copy_val = (unsigned int)floor(*static_cast<const float*>(val));
			break;
		case ValueType::String:
		{
			const std::string copy_val_str = *static_cast<const std::string*>(val);
			if (is_num_int(copy_val_str))
				copy_val = (unsigned int)std::stoi(copy_val_str);
			break;
		}
		case ValueType::Analogous:
			try
			{
				copy_val = *static_cast<const unsigned int*>(val);
			}
			catch (const std::bad_cast&)
			{

			}
			break;
		default:
			break;
		}

		POPRETURN_CLEANUP
		return copy_val;
	}

	template<> BISMUTH_API
	std::string state::popFromReturn()
	{
		POPRETURN_INITIALIZE

		std::string copy_val = "";
		switch (top.Type)
		{
		case ValueType::String:
			copy_val = *static_cast<const std::string*>(val);
			break;
		case ValueType::Uint:
		case ValueType::Int:
			copy_val = std::to_string(*static_cast<const int*>(val));
			break;
		case ValueType::Float:
			copy_val = std::to_string(*static_cast<const float*>(val));
			break;
		case ValueType::Nil:
			copy_val = "NIL";
			break;
		case ValueType::Custom:
			// TODO: Class to string methods
			break; 
		case ValueType::Analogous:
			try
			{
				copy_val = *static_cast<const std::string*>(val);
			}
			catch (const std::bad_cast&)
			{

			}
			break;
		default:
			break;
		}

		POPRETURN_CLEANUP
		return copy_val;
	}

	template<>
	float state::popFromReturn()
	{
		POPRETURN_INITIALIZE

		float copy_val = NAN;
		switch (top.Type)
		{
		case ValueType::Float:
			copy_val = *static_cast<const float*>(val);
			break;
		case ValueType::Int:
		case ValueType::Uint:
			copy_val = (float)*(static_cast<const int*>(val));
			break;
		case ValueType::String:
		{
			const std::string copy_val_str = *static_cast<const std::string*>(val);
			if (is_num_float(copy_val_str))
				copy_val = std::stof(copy_val_str);
			break;
		}
		case ValueType::Analogous:
			try
			{
				copy_val = *static_cast<const float*>(val);
			}
			catch (const std::bad_cast&)
			{

			}
			break;
		default:
			break;
		}

		POPRETURN_CLEANUP
		return copy_val;
	}

	template<>
	const void* state::popFromReturn()
	{
		POPRETURN_INITIALIZE

		--m_ReturnsStackTop;
		return val;
	}
	




	std::vector<BismuthFunctionInput> state::GetInputs(const std::vector<token>& tokens, unsigned int& i) const
	{
		std::vector<BismuthFunctionInput> inputs;
		while (tokens[(++i)].Type != tokenType::ExpressionEnd)
		{
			token c_token = tokens[i];

			if (c_token.Type == tokenType::Identifier)
			{
				const std::string inputType = c_token.Value.value();
				ValueType type;

				if (inputType == "int")
					type = ValueType::Int;
				else if (inputType == "str")
					type = ValueType::String;
				else if (inputType == "float")
					type = ValueType::Float;
				else if (inputType == "uint")
					type = ValueType::Uint;
				else if (inputType == "any")
					type = ValueType::Analogous;
				else
					type = ValueType::Custom;

				++i;
				c_token = tokens[i];
				if (c_token.Type == tokenType::Identifier)
				{
					const std::string inputName = c_token.Value.value();
					inputs.push_back(BismuthFunctionInput(type, inputName, (type == ValueType::Custom ? (std::optional<std::string>)inputType : std::nullopt)));
				}
			}

		}
		++i;
		return inputs;
	}


	void state::Build(const std::vector<token>& tokens)
	{
		unsigned int i{};
		while (i < tokens.size())
		{
			token c_token = tokens[i];
			/*
			*	Identifier: Function
			*/
			if (c_token.Type == tokenType::Identifier)
			{
				const std::string type_str = c_token.Value.value();

				ValueType returnType = (type_str == "int" ? ValueType::Int : type_str == "str" ? ValueType::String : type_str == "float" ? ValueType::Float : type_str == "uint" ? ValueType::Uint : type_str == "any" ? ValueType::Analogous : ValueType::Custom);

					
				++i; // off of return identifier
				c_token = tokens[i];
				if (c_token.Type == tokenType::Identifier)
				{
					const std::string functionName = c_token.Value.value();
					++i;

					std::vector<BismuthFunctionInput> inputs(GetInputs(tokens, i));
					std::reverse(inputs.begin(), inputs.end());

					c_token = tokens[i];
					if (c_token.Type == tokenType::ScopeStart)
					{
						++i; // to first token
						std::vector<token> func_tokens;
						unsigned int stack = 1;
						while (stack != 0)
						{
							c_token = tokens[i];

							if (c_token.Type == tokenType::ScopeStart)
								++stack;
							else if (c_token.Type == tokenType::ScopeEnd)
								--stack;
							else func_tokens.push_back(c_token);

							++i;
						}

						BismuthFunction_Local* func = new BismuthFunction_Local(returnType, inputs, func_tokens, (returnType == ValueType::Custom ? (std::optional<std::string>)type_str : std::nullopt));
						m_GlobalFunctions[functionName] = func;
					}
				}
			}

			/*
			* Class Declaration: "
			*/
			else if (c_token.Type == tokenType::ClassDecl)
			{
				if (tokens[i + 1].Type == tokenType::Identifier)
				{
					++i; // to identifier
					c_token = tokens[i];
					const std::string className = c_token.Value.value();

					std::shared_ptr<BismuthClassTemplate> parent = nullptr;
					if (tokens[i + 1].Type == tokenType::Inherite)
					{
						i += 2; // to inherited class ident
						c_token = tokens[i];
						if (c_token.Type == tokenType::Identifier)
						{
							const std::string inheritedIdentifier = c_token.Value.value();

							if (m_ClassTypes.count(inheritedIdentifier) > 0)
							{
								parent = m_ClassTypes[inheritedIdentifier];
							}

						}
					}


					if (tokens[i + 1].Type == tokenType::ScopeStart)
					{
						++i;
						c_token = tokens[i];

						std::vector<PropertyTemplate> properties;

						while (tokens[i].Type != tokenType::ScopeEnd)
						{
							++i;
							c_token = tokens[i];

							if (c_token.Type == tokenType::PublicDomain || c_token.Type == tokenType::ProtectedDomain || c_token.Type == tokenType::PrivateDomain)
							{
								BismuthClassDomain domain = (c_token.Type == tokenType::PublicDomain ? BismuthClassDomain::Public : c_token.Type == tokenType::ProtectedDomain ? BismuthClassDomain::Protected : BismuthClassDomain::Private);

								++i;
								c_token = tokens[i];
								/*
								* Function return type or variable type
								*/
								if (c_token.Type == tokenType::Identifier)
								{

									const std::string& type_str = c_token.Value.value();
									ValueType valType = (type_str == "int" ? ValueType::Int : type_str == "str" ? ValueType::String : type_str == "float" ? ValueType::Float : type_str == "uint" ? ValueType::Uint : type_str == "any" ? ValueType::Analogous : ValueType::Custom);

									++i;
									c_token = tokens[i];
									if (c_token.Type == tokenType::Identifier)
									{
										const std::string propertyName = c_token.Value.value();

										++i;
										c_token = tokens[i];

										/*
										* Followed by expression: Function
										* Else: Variable
										*/
										if (c_token.Type == tokenType::ExpressionStart)
										{
											// TODO: Class methods
										}
										else
										{
											properties.push_back({valType, domain, propertyName});
										}
									}
								}
							}
						}

						m_ClassTypes[className] = std::make_shared<BismuthClassTemplate>(properties, parent);
						++i;
					}
				}
			}

			++i;
		}
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
				pushToReturn(new int(std::stoi(c_token.Value.value())), ValueType::Int, true);
			else if (c_token.Type == tokenType::StaticFloat)
				pushToReturn(new float(std::stof(c_token.Value.value())), ValueType::Float, true);
			else if (c_token.Type == tokenType::StaticString)
				pushToReturn(new std::string(c_token.Value.value()), ValueType::String);
			else if (c_token.Type == tokenType::StaticNil)
				pushToReturn(nullptr, ValueType::Nil);
			else if (c_token.Type == tokenType::Identifier)
			{
				const std::string identifierName = c_token.Value.value();
				
				if (tokens[i + 1].Type == tokenType::ExpressionStart) // call function + push return
				{
					if (m_GlobalFunctions.count(identifierName) > 0 && m_GlobalFunctions[identifierName]->m_FunctionReturnType != ValueType::Nil)
					{
						EvaluateExpression(tokens, i);
						RunFunction(m_GlobalFunctions[identifierName]); 
					}
				}
				else // push variable value to stack
				{
					std::optional<IBismuthVariable*> var_opt = GetVariable(identifierName);

					if (var_opt.has_value())
					{
						IBismuthVariable* var = var_opt.value();
						pushToReturn(var->GetAnalogous(), var->GetStaticType());
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
						var = new bis_int(ValueType::Int, 0);
					else if (varType == "str")
						var = new bis_string(ValueType::String, "");
					else if (varType == "float")
						var = new bis_float(ValueType::Float, 0.0f);
					else if (varType == "uint")
						var = new bis_uint(ValueType::Uint, 0U);
					else if (varType == "any")
						var = new bis_voidptr(ValueType::Analogous, nullptr);

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
						else if (varType == "str")
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
						else if (varType == "any")
						{
							bis_voidptr* var_voidptr = dynamic_cast<bis_voidptr*>(var);
							var_voidptr->Set(popFromReturn<const void*>());
						}



					} // else instantiate null

					PushVariable(c_token.Value.value(), std::move(var));

				}
				else if (tokens[i + 1].Type == tokenType::Assignment) // var change
				{
					const std::string varName = c_token.Value.value();

					std::optional<IBismuthVariable*> var_opt = GetVariable(varName);

					if (var_opt.has_value())
					{
						IBismuthVariable* var = var_opt.value();
						const ValueType varType = var->GetStaticType();

						EvaluateExpression(tokens, i);
						if (varType == ValueType::Int)
						{
							bis_int* var_int = dynamic_cast<bis_int*>(var);
							var_int->Set(popFromReturn<int>());
						}
						else if (varType == ValueType::String)
						{
							bis_string* var_string = dynamic_cast<bis_string*>(var);
							var_string->Set(popFromReturn<std::string>());
						}
						else if (varType == ValueType::Float)
						{
							bis_float* var_float = dynamic_cast<bis_float*>(var);
							var_float->Set(popFromReturn<float>());
						}
						else if (varType == ValueType::Uint)
						{
							bis_uint* var_uint = dynamic_cast<bis_uint*>(var);
							var_uint->Set(popFromReturn<unsigned int>());
						}
						else if (varType == ValueType::Analogous)
						{
							bis_voidptr* var_voidptr = dynamic_cast<bis_voidptr*>(var);
							var_voidptr->Set(popFromReturn<const void*>());
						}
					}
					
				}
				else if (tokens[i + 1].Type == tokenType::ExpressionStart) // function call
				{
					const std::string identifierName = c_token.Value.value();

					if (m_GlobalFunctions.count(identifierName) > 0)
					{
						EvaluateExpression(tokens, i);
						RunFunction(m_GlobalFunctions[identifierName]);
					}

				}

			}

			/* scope start + end: push or pop new scopes*/
			else if (c_token.Type == tokenType::ScopeStart)
				PushScope();
			else if (c_token.Type == tokenType::ScopeEnd)
				PopScope();
			else if (c_token.Type == tokenType::Return)
			{
				EvaluateExpression(tokens, i);
				return;
			}

			++i;
		}
	}


	void state::DoString(const std::string& source)
	{
		m_OperationThread = std::thread([this](const std::string& src){
			PushScope();
			Evaluate(tokenize(src));
			PopScope();
		}, source);
	}

	void state::RunFunction(const IBismuthFunction* const function)
	{
		PushScope();

		for (const BismuthFunctionInput& c_input : function->m_FunctionInputs)
		{
			switch (c_input.Type)
			{
			case ValueType::Int:
				PushVariable<int>(c_input.Name, ValueType::Int, popFromReturn<int>());
				break;
			case ValueType::String:
				PushVariable<std::string>(c_input.Name, ValueType::String, popFromReturn<std::string>());
				break;
			case ValueType::Float:
				PushVariable<float>(c_input.Name, ValueType::Float, popFromReturn<float>());
				break;
			case ValueType::Uint:
				PushVariable<unsigned int>(c_input.Name, ValueType::Uint, popFromReturn<unsigned int>());
				break;
			case ValueType::Analogous:
				PushVariable<const void*>(c_input.Name, ValueType::Analogous, popFromReturn<const void*>());
				break;
			case ValueType::Custom:
				// TODO: custom types
				break;
			default:
				break;
			}
		}

		switch (function->GetRelativity())
		{
		case BismuthFunctionRelativity::Local:
		{
			const BismuthFunction_Local* const func_local = dynamic_cast<const BismuthFunction_Local* const>(function);
			Evaluate(func_local->m_Tokens);
			break;
		}
		case BismuthFunctionRelativity::Native:
		{
			const BismuthFunction_Native* const func_native = dynamic_cast<const BismuthFunction_Native* const>(function);
			func_native->m_NativeFunction(this);
			break;
		}
		case BismuthFunctionRelativity::NONE:
			break;
		}
		PopScope();
	}






	BismuthClassTemplate::BismuthClassTemplate(const std::vector<PropertyTemplate>& properties, const std::shared_ptr<BismuthClassTemplate> parent)
		: m_ParentClass(parent)
		, m_TotalProperties(properties.size())
	{
		unsigned int offset = 0;
		for (const auto& prop : properties)
		{
			m_Properties[prop.Name] = { prop.type, offset, prop.domain };
		}
	}

	const std::optional<BismuthClassProperty> BismuthClassTemplate::getStaticProperty(const std::string& name, bool local) const
	{
		if (m_Properties.count(name) > 0 && (m_Properties.at(name).Domain == BismuthClassDomain::Public || m_Properties.at(name).Domain == BismuthClassDomain::Protected || local))
			return m_Properties.at(name);
		else if (m_ParentClass)
			return m_ParentClass->getStaticProperty(name, false).value();
		else
			return std::nullopt;
	}

	std::shared_ptr<BismuthClassTemplate> BismuthClassTemplate::CreateClassTemplate(const std::vector<PropertyTemplate>& properties, const std::shared_ptr<BismuthClassTemplate> parent)
	{
		return std::make_shared<BismuthClassTemplate>(properties, parent);
	}



	BismuthClassInstance::BismuthClassInstance(const std::shared_ptr<BismuthClassTemplate> _template)
		: IBismuthClassInstance(BismuthClassInstanceRelativity::Templated)
		, m_ClassTemplate(_template)
	{
		m_Properties = (const void**)malloc(_template->m_TotalProperties * sizeof(const void*));
	}

	template<typename _T>
	const std::optional<_T> BismuthClassInstance::getProperty(const std::string& name) const
	{
		std::optional<BismuthClassProperty> prop = m_ClassTemplate->getStaticProperty(name);
		if (prop.has_value())
		{
			const void* val = m_Properties[prop.value().Offset];
			return *static_cast<const _T*>(val);
		}
		else return std::nullopt;
	}

	#define CLASSINSTANCE_GETPROP_INITIALIZE std::optional<BismuthClassProperty> prop = m_ClassTemplate->getStaticProperty(name, true);\
											if (prop.has_value())
	#define CLASSINSTANCE_GETPROP_CLEANUP else return std::nullopt;

	template<> BISMUTH_API
	const std::optional<int> BismuthClassInstance::getProperty(const std::string& name) const
	{
		CLASSINSTANCE_GETPROP_INITIALIZE
		{
			const void* val = m_Properties[prop.value().Offset];

			switch (prop.value().Type)
			{
			case ValueType::Int:
				return *static_cast<const int*>(val);
			case ValueType::Uint:
				return (int)*static_cast<const unsigned int*>(val);
			case ValueType::String:
			{
				const std::string* as_str = static_cast<const std::string*>(val);
				if (state::is_num_int(*as_str))
					return std::stoi(*as_str);
				else return (int)NAN;
			}
			case ValueType::Float:
				return (int)*static_cast<const float*>(val);
			case ValueType::Analogous:
				try
				{
					int as_int = *static_cast<const int*>(val);
				}
				catch(const std::bad_cast&)
				{
					return 0;
				}
			default:
				return 0;
			}
		}
		CLASSINSTANCE_GETPROP_CLEANUP
	}

	template<> BISMUTH_API
	const std::optional<std::string> BismuthClassInstance::getProperty(const std::string& name) const
	{
		CLASSINSTANCE_GETPROP_INITIALIZE
		{
			const void* val = m_Properties[prop.value().Offset];

			switch (prop.value().Type)
			{
			case ValueType::String:
				return *static_cast<const std::string*>(val);
			case ValueType::Int:
			case ValueType::Uint:
				return std::to_string(*static_cast<const int*>(val));
			case ValueType::Float:
				return std::to_string(*static_cast<const float*>(val));
			case ValueType::Analogous:
				try
				{
					return *static_cast<const std::string*>(val);
				}
				catch (const std::bad_cast&)
				{
					return "NIL";
				}
			default:
				return "NIL";
			}
		}
		CLASSINSTANCE_GETPROP_CLEANUP
	}

	template<> BISMUTH_API
	const std::optional<float> BismuthClassInstance::getProperty(const std::string& name) const
	{
		CLASSINSTANCE_GETPROP_INITIALIZE
		{
			const void* val = m_Properties[prop.value().Offset];

			switch(prop.value().Type)
			{
			case ValueType::Float:
				return *static_cast<const float*>(val);
			case ValueType::Int:
				return (int)*static_cast<const int*>(val);
			case ValueType::Uint:
				return (int)*static_cast<const unsigned int*>(val);
			case ValueType::String:
			{
				const std::string* as_str = static_cast<const std::string*>(val);
				if (state::is_num_float(*as_str))
					return std::stof(*as_str);
				else 
					return 0;
			}
			case ValueType::Analogous:
				try
				{
					return *static_cast<const float*>(val);
				}
				catch (const std::bad_cast&)
				{
					return 0;
				}
			default:
				return 0;
			}
		}
		CLASSINSTANCE_GETPROP_CLEANUP
	}

	template<> BISMUTH_API
	const std::optional<unsigned int> BismuthClassInstance::getProperty(const std::string& name) const
	{
		CLASSINSTANCE_GETPROP_INITIALIZE
		{
			const void* val = m_Properties[prop.value().Offset];

			switch (prop.value().Type)
			{
			case ValueType::Uint:
				return *static_cast<const unsigned int*>(val);
			case ValueType::Int:
				return (unsigned)*static_cast<const int*>(val);
			case ValueType::String:
			{
				const std::string* as_str = static_cast<const std::string*>(val);
				if (state::is_num_int(*as_str))
					return (unsigned)std::stoi(*as_str);
				else return 0;
			}
			case ValueType::Analogous:
				try
				{
					return *static_cast<const unsigned int*>(val);
				}
				catch (const std::bad_cast&)
				{
					return 0;
				}
			default:
				return 0;
			}
		}
		CLASSINSTANCE_GETPROP_CLEANUP
	}

	template<> BISMUTH_API
	const std::optional<const void*> BismuthClassInstance::getProperty(const std::string& name) const
	{
		CLASSINSTANCE_GETPROP_INITIALIZE
		{
			return m_Properties[prop.value().Offset];
		}
		CLASSINSTANCE_GETPROP_CLEANUP
	}


	template<typename _T>
	void BismuthClassInstance::setProperty(const std::string& name, _T newValue)
	{
		std::optional<BismuthClassProperty> prop = m_ClassTemplate->getStaticProperty(name, true);
		if (prop.has_value())
		{
			m_Properties[prop.value().Offset] = &newValue;
		}
	}



	BismuthClassInstace_UserHandle::BismuthClassInstace_UserHandle(const std::vector<std::pair<std::string, BismuthUserHandle>>& handles)
		: IBismuthClassInstance(BismuthClassInstanceRelativity::UserHandle)
	{
		for (const auto& handle : handles)
		{
			m_Properties[handle.first] = handle.second;
		}
	}

	template<typename _T>
	const std::optional<_T> BismuthClassInstace_UserHandle::getProperty(const std::string& name) const
	{
		return std::nullopt;
	}

	template<> BISMUTH_API
	const std::optional<int> BismuthClassInstace_UserHandle::getProperty(const std::string& name) const
	{
		if (m_Properties.count(name) > 0)
		{
			const void* const val = m_Properties.at(name).Target;

			switch (m_Properties.at(name).Type)
			{
			case ValueType::Int:
				return *static_cast<const int*>(val);
			case ValueType::Uint:
				return (int)*static_cast<const unsigned int*>(val);
			case ValueType::String:
			{
				const std::string* as_str = static_cast<const std::string*>(val);
				if (state::is_num_int(*as_str))
					return std::stoi(*as_str);
				else
					return 0;
			}
			case ValueType::Float:
				return (int)*static_cast<const float*>(val);
			case ValueType::Analogous:
				try
				{
					return *static_cast<const int*>(val);
				}
				catch(const std::bad_cast&)
				{
					return 0;
				}
			default:
				return 0;
			}
		}
		return std::nullopt;
	}

	template<> BISMUTH_API
	const std::optional<std::string> BismuthClassInstace_UserHandle::getProperty(const std::string& name) const
	{
		if (m_Properties.count(name) > 0)
		{
			const void* const val = m_Properties.at(name).Target;

			switch (m_Properties.at(name).Type)
			{
			case ValueType::String:
				return *static_cast<const std::string*>(val);
			case ValueType::Int:
				return std::to_string(*static_cast<const int*>(val));
			case ValueType::Uint:
				return std::to_string(*static_cast<const unsigned int*>(val));
			case ValueType::Float:
				return std::to_string(*static_cast<const float*>(val));
			case ValueType::Analogous:
				try
				{
					return *static_cast<const std::string*>(val);
				}
				catch (const std::bad_cast&)
				{
					return "NIL";
				}
			default:
				return "NIL";
			}
		}
		return std::nullopt;
	}

	template<> BISMUTH_API
	const std::optional<unsigned int> BismuthClassInstace_UserHandle::getProperty(const std::string& name) const
	{
		if (m_Properties.count(name) > 0)
		{
			const void* const val = m_Properties.at(name).Target;

			switch (m_Properties.at(name).Type)
			{
			case ValueType::Uint:
				return *static_cast<const unsigned int*>(val);
			case ValueType::Int:
				return (unsigned)*static_cast<const int*>(val);
			case ValueType::String:
			{
				const std::string* as_str = static_cast<const std::string*>(val);
				if (state::is_num_int(*as_str))
					return std::stoi(*as_str);
				else
					return 0;
			}
			case ValueType::Float:
				return (unsigned int)*static_cast<const float*>(val);
			case ValueType::Analogous:
				try
				{
					return *static_cast<const unsigned int*>(val);
				}
				catch (const std::bad_cast&)
				{
					return 0;
				}
			default:
				return 0;
			}
		}
		else return std::nullopt;
	}

	template<> BISMUTH_API
	const std::optional<float> BismuthClassInstace_UserHandle::getProperty(const std::string& name) const
	{
		if (m_Properties.count(name) > 0)
		{
			const void* const val = m_Properties.at(name).Target;

			switch (m_Properties.at(name).Type)
			{
			case ValueType::Float:
				return *static_cast<const float*>(val);
			case ValueType::Int:
				return (float)*static_cast<const int*>(val);
			case ValueType::Uint:
				return (float)*static_cast<const unsigned int*>(val);
			case ValueType::String:
			{
				const std::string* as_str = static_cast<const std::string*>(val);
				if (state::is_num_float(*as_str))
					return std::stof(*as_str);
				else
					return 0;
			}
			case ValueType::Analogous:
				try
				{
					return *static_cast<const float*>(val);
				}
				catch (const std::bad_cast&)
				{
					return 0;
				}
			default:
				return 0;
			}
		}
		else return std::nullopt;
	}

	template<typename _T> 
	void BismuthClassInstace_UserHandle::setProperty(const std::string& name, _T newValue)
	{
		if (m_Properties.count(name) > 0)
			m_Properties[name].Target = &newValue;
	}


	template<typename _T>
	const std::optional<_T> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const instance, const std::string& name)
	{
		switch (instance->GetStaticRelativity())
		{
		case BismuthClassInstanceRelativity::Templated:
		{
			const BismuthClassInstance* const inst = dynamic_cast<const BismuthClassInstance* const>(instance);
			return inst->getProperty<_T>(name);
		}
		case BismuthClassInstanceRelativity::UserHandle:
		{
			const BismuthClassInstace_UserHandle* const inst = dynamic_cast<const BismuthClassInstace_UserHandle* const>(instance);
			return inst->getProperty<_T>(name);
		}
		}
	}

	template<typename _T>
	void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const instance, const std::string& name, _T newValue)
	{
		switch (instance->GetStaticRelativity())
		{
		case BismuthClassInstanceRelativity::Templated:
		{
			BismuthClassInstance* const inst = dynamic_cast<BismuthClassInstance* const>(instance);
			inst->setProperty<_T>(name, newValue);
			break;
		}
		case BismuthClassInstanceRelativity::UserHandle:
		{
			BismuthClassInstace_UserHandle* const inst = dynamic_cast<BismuthClassInstace_UserHandle* const>(instance);
			inst->setProperty<_T>(name, newValue);
			break;
		}
		}
	}


}