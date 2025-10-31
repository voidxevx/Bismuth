#include "state.h"

#include <sstream>
#include <iostream>

namespace bis
{

	state::state()
	{
	}

	state::~state()
	{
	}

	void
	state::PushVariable(size_t id, __IVariable* var, bool _mutable)
	{
		ValueType::typeHint type = ValueType::__GetHint(var->GetType()).hint;
		if (type == ValueType::typeHint::Class)
		{
			__ClassVariable* _class = dynamic_cast<__ClassVariable*>(var);
			m_CurrentScope->InsertVariable(id);
			m_VarStack.push(_class);
			const auto& props = _class->GetTemplate()->GetProperties();
			for (const auto& prop : props)
			{
				m_VarStack.push(NullType(prop.second.Type, _mutable));
				m_CurrentScope->InsertProperty();
			}
		}
		else
		{
			m_CurrentScope->InsertVariable(id);
			m_VarStack.push(var);
		}
	}

	std::optional<__IVariable*> state::GetRawVariable(size_t id)
	{
		std::optional<const unsigned int> loc = m_CurrentScope->GetVariableLocation(id);
		if (loc.has_value())
			return m_VarStack[loc.value()];
		else
			return std::nullopt;
	}

	template<typename _T> std::optional<_T>
	state::GetVariable(size_t id)
	{
		std::optional<__IVariable*> var_opt = GetRawVariable(id);
		if (var_opt.has_value())
			return __SmartCast<_T>(var_opt.value());
		else
			return std::nullopt;
	}


	void
	state::PushScope(const unsigned int root)
	{
		Scope* newScope = new Scope(m_CurrentScope, root);
		m_CurrentScope = newScope;
	}

	void
	state::PopScope()
	{
		const size_t deallocAmount = m_CurrentScope->GetScopeSize();
		for (unsigned int i{}; i < deallocAmount; ++i)
		{
			m_VarStack.pop();
		}
		Scope* nextScope = m_CurrentScope->GetParentScope();
		delete m_CurrentScope;
		m_CurrentScope = nextScope;
	}

	void state::PushTokens(token* const newTokens)
	{
		if (m_TopToken)
			m_TopToken->append(newTokens);
		else
			m_TopToken = newTokens;
	}

	void state::popToken()
	{
		m_TopToken = m_TopToken->Next;
	}

	std::optional<const unsigned int> state::TraceIdentifier()
	{

		__ClassVariable* currentClass = nullptr;
		unsigned int currentLocation = 0;
		PropertyAccess::Point accessPoint = PropertyAccess::Point::Indirect;

		if (m_TopToken)
		{

			if (m_TopToken->Type == tokenType::Identifier)
			{
				std::optional<const unsigned int> loc_opt = m_CurrentScope->GetVariableLocation(m_TopToken->ID.value());
				if (loc_opt.has_value())
				{
					currentLocation = loc_opt.value();
					__IVariable* c_var = m_VarStack[currentLocation];
					ValueType::typeHint type = ValueType::__GetHint(c_var->GetType()).hint;

					if (type == ValueType::typeHint::Class)
					{
						currentClass = dynamic_cast<__ClassVariable*>(c_var);
					}
					else // primitive types can't have properties
						return currentLocation;
				}
				else return std::nullopt;

			}
			else return std::nullopt;

			
			popToken(); // pop identifier
			/*
			* trace loop
			*/
			while (m_TopToken && m_TopToken->Type == tokenType::Property)
			{
				popToken(); // pop property token

				if (m_TopToken->Type == tokenType::Identifier)
				{
					
					const size_t id = m_TopToken->ID.value();

					std::optional<PropertyLocation> offset = currentClass->GetTemplate()->GetStaticProperty(id, accessPoint);

					if (offset.has_value())
					{
						currentLocation -= offset.value().Offset;
						__IVariable* currentVar = m_VarStack[currentLocation];

						ValueType::typeHint type = ValueType::__GetHint(currentVar->GetType()).hint;
						if (type == ValueType::typeHint::Class)
						{
							currentClass = dynamic_cast<__ClassVariable*>(currentVar);
						}
						else return currentLocation;

						if (accessPoint == PropertyAccess::Point::Direct)
							accessPoint = PropertyAccess::Point::Indirect;
					}
					else return currentLocation;
				}
				else return currentLocation;

				popToken(); // pop identifier
			}

			return currentLocation;

		}
		else return std::nullopt;

	}


	token* state::tokenize(const std::string& src)
	{
		token* current = new token(nullptr, nullptr, tokenType::Start);
		token* const first = current;

		unsigned long i{};
		while (i < src.size())
		{

			if (!std::isspace(src[i]) || src[i] == '\n' && i < src.size())
			{
				if (src[i] == '\n')
				{
					current->Next = new token(nullptr, current, tokenType::LineEnd);
					current = current->Next;
					++i;
					continue;
				}


				std::stringstream buffer;
				if (std::isalnum(src[i]))
				{
					while (std::isalnum(src[i]) || src[i] == '.')
					{
						buffer << src[i];
						++i;
					}

					const std::string str = buffer.str();
					if (str == "mutable")
					{
						current->Next = new token(nullptr, current, tokenType::MutableType);
					}
					else if (str == "const")
					{
						current->Next = new token(nullptr, current, tokenType::ConstType);
					}
					else if (str == "public")
					{
						current->Next = new token(nullptr, current, tokenType::PublicDomain);
					}
					else if (str == "protected")
					{
						current->Next = new token(nullptr, current, tokenType::ProtectedDomain);
					}
					else if (str == "private")
					{
						current->Next = new token(nullptr, current, tokenType::PrivateDomain);
					}
					else if (str == "class")
					{
						current->Next = new token(nullptr, current, tokenType::ClassDecl);
					}
					else if (str == "extends")
					{
						current->Next = new token(nullptr, current, tokenType::Inheritance);
					}
					else if (str == "true")
					{
						current->Next = new token(nullptr, current, tokenType::StaticBoolTrue);
					}
					else if (str == "false")
					{
						current->Next = new token(nullptr, current, tokenType::StaticBoolFalse);
					}
					else if (str == "null")
					{
						current->Next = new token(nullptr, current, tokenType::StaticNull);
					}
					else if (str[str.size() - 1] == 'l' && is_long(str.substr(0, str.size() - 2)))
					{
						const std::string val = str.substr(0, str.size() - 2);
						const size_t id = e_IdentityHasher(val);
						current->Next = new token(nullptr, current, tokenType::StaticLong, id);
						m_Constants[id] = val;
					}
					else if (str[str.size() - 1] == 'f' && is_float(str.substr(0, str.size() - 2)))
					{
						const std::string val = str.substr(0, str.size() - 2);
						const size_t id = e_IdentityHasher(val);
						current->Next = new token(nullptr, current, tokenType::StaticFloat, id);
						m_Constants[id] = val;
					}
					else if (str[str.size() - 1] == 'd' && is_double(str.substr(0, str.size() - 2)))
					{
						const std::string val = str.substr(0, str.size() - 2);
						const size_t id = e_IdentityHasher(val);
						current->Next = new token(nullptr, current, tokenType::StaticDouble, id);
						m_Constants[id] = val;
					}
					else if (is_int(str))
					{
						const size_t id = e_IdentityHasher(str);
						current->Next = new token(nullptr, current, tokenType::StaticInt, id);
						m_Constants[id] = str;
					}
					else
					{
						const size_t id = e_IdentityHasher(str);
						current->Next = new token(nullptr, current, tokenType::Identifier, id);
						m_Constants[id] = str;
					}

					if (current->Next)
						current = current->Next;
				}
				else
				{
					while (!std::isalnum(src[i]) && !std::isspace(src[i]) && i < src.size())
					{
						if (src[i] == '"')
						{
							++i;
							while (src[i] != '"');
							{
								buffer << src[i];
								++i;
							}
						}
						else if (src[i] == '\'')
						{
							++i;
							while (src[i] != '"');
							{
								buffer << src[i];
								++i;
							}
						}
						else
						{
							buffer << src[i];
							++i;
						}
					}

					const std::string str = buffer.str();
					if (str == "(")
					{
						current->Next = new token(nullptr, current, tokenType::ExpressionStart);
					}
					else if (str == ")")
					{
						current->Next = new token(nullptr, current, tokenType::ExpressionEnd);
					}
					else if (str == ",")
					{
						current->Next = new token(nullptr, current, tokenType::ExpressionBreak);
					}
					else if (str == "{")
					{
						current->Next = new token(nullptr, current, tokenType::ScopeStart);
					}
					else if (str == "}")
					{
						current->Next = new token(nullptr, current, tokenType::ScopeEnd);
					}
					else if (str == ";")
					{
						current->Next = new token(nullptr, current, tokenType::LineEnd);
					}
					else if (str == ":")
					{
						current->Next = new token(nullptr, current, tokenType::Property);
					}
					else if (str == "=")
					{
						current->Next = new token(nullptr, current, tokenType::Assignement);
					}
					else if (str[0] == '"')
					{
						const size_t id = e_IdentityHasher(str);
						current->Next = new token(nullptr, current, tokenType::StaticString, id);
						m_Constants[id] = str;
					}
					else if (str[0] == '\'' && str.size() <= 3)
					{
						const size_t id = e_IdentityHasher(str);
						current->Next = new token(nullptr, current, tokenType::StaticByte, id);
						m_Constants[id] = str;
					}

					if (current->Next)
						current = current->Next;

				}

			}
			else ++i;

		}

		current->Next = new token(nullptr, current, tokenType::End);
		current = current->Next;

		return first;
	}

	void state::Evaluate()
	{

		while ((m_TopToken = m_TopToken->Next) && m_TopToken->Type != tokenType::End)
		{

			/*
			* mutable or const: variable instantiation
			*/
			if (m_TopToken->Type == tokenType::MutableType || m_TopToken->Type == tokenType::ConstType)
			{
				const bool _const = m_TopToken->Type == tokenType::ConstType;
				m_TopToken = m_TopToken->Next;
				if (m_TopToken->Type == tokenType::Identifier)
				{
					if (m_TopToken->Next->Type == tokenType::Identifier)
					{
						ValueType::ValueType type;
						if (m_TopToken->ID.value() == e_IdentityHasher("int"))
							type = ValueType::__ValueType(ValueType::Int, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("uint"))
							type = ValueType::__ValueType(ValueType::Uint, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("long"))
							type = ValueType::__ValueType(ValueType::Long, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("ulong"))
							type = ValueType::__ValueType(ValueType::Ulong, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("float"))
							type = ValueType::__ValueType(ValueType::Float, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("double"))
							type = ValueType::__ValueType(ValueType::Double, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("byte"))
							type = ValueType::__ValueType(ValueType::Byte, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("ubyte"))
							type = ValueType::__ValueType(ValueType::Ubyte, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("bool"))
							type = ValueType::__ValueType(ValueType::Bool, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("str"))
							type = ValueType::__ValueType(ValueType::String, ValueType::Value);
						else if (m_TopToken->ID.value() == e_IdentityHasher("null_t"))
							type = ValueType::__ValueType(ValueType::Null, ValueType::Value);
						else
							type = ValueType::__ValueType(ValueType::Class, ValueType::Value);


						m_TopToken = m_TopToken->Next;
						const size_t id = m_TopToken->ID.value();
						PushVariable(id, NullType(type, !_const), !_const);


						/* Instantiate with value */
						m_TopToken = m_TopToken->Next;
						if (m_TopToken->Type == tokenType::Assignement)
						{
							m_TopToken = m_TopToken->Next;
							EvaluateExpression(1);
							switch (ValueType::__GetHint(type).hint)
							{
							case ValueType::Int:
								dynamic_cast<bisInt*>(m_VarStack[0])->Set(__SmartCast<int>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Uint:
								dynamic_cast<bisUint*>(m_VarStack[0])->Set(__SmartCast<unsigned int>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Long:
								dynamic_cast<bisLong*>(m_VarStack[0])->Set(__SmartCast<long>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Ulong:
								dynamic_cast<bisUlong*>(m_VarStack[0])->Set(__SmartCast<unsigned long>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Float:
								dynamic_cast<bisFloat*>(m_VarStack[0])->Set(__SmartCast<float>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Double:
								dynamic_cast<bisDouble*>(m_VarStack[0])->Set(__SmartCast<double>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Byte:
								dynamic_cast<bisByte*>(m_VarStack[0])->Set(__SmartCast<char>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Ubyte:
								dynamic_cast<bisUbyte*>(m_VarStack[0])->Set(__SmartCast<unsigned char>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Bool:
								dynamic_cast<bisBool*>(m_VarStack[0])->Set(__SmartCast<bool>(m_ReturnStack.pop()), true);
								break;
							case ValueType::String:
								dynamic_cast<bisString*>(m_VarStack[0])->Set(__SmartCast<std::string>(m_ReturnStack.pop()), true);
								break;
							case ValueType::Null:
								break;
							default:
								break;
							}
						}
					}

				}
				else return;

			}

			/*
			* Identifier:
			*	- Variable assignment
			*	- Function Call
			*/
			else if (m_TopToken->Type == tokenType::Identifier)
			{
				std::optional<const unsigned int> loc_opt = TraceIdentifier();
				/*
				* change variable value
				*/
				if (loc_opt.has_value())
				{
					ValueType::typeHint type = ValueType::__GetHint(m_VarStack[loc_opt.value()]->GetType()).hint;
					EvaluateExpression(1);
					switch (ValueType::__GetHint(type).hint)
					{
					case ValueType::Int:
						dynamic_cast<bisInt*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<int>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Uint:
						dynamic_cast<bisUint*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<unsigned int>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Long:
						dynamic_cast<bisLong*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<long>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Ulong:
						dynamic_cast<bisUlong*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<unsigned long>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Float:
						dynamic_cast<bisFloat*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<float>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Double:
						dynamic_cast<bisDouble*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<double>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Byte:
						dynamic_cast<bisByte*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<char>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Ubyte:
						dynamic_cast<bisUbyte*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<unsigned char>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Bool:
						dynamic_cast<bisBool*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<bool>(m_ReturnStack.pop()), true);
						break;
					case ValueType::String:
						dynamic_cast<bisString*>(m_VarStack[loc_opt.value()])->Set(__SmartCast<std::string>(m_ReturnStack.pop()), true);
						break;
					case ValueType::Null:
						break;
					default:
						break;
					}
				}

				/*
				* Function call
				*/
				else
				{
					if (m_TopToken->ID == e_IdentityHasher("print"))
					{
						EvaluateExpression(1);
						const std::string as_str = __SmartCast<std::string>(m_ReturnStack.pop());
						std::cout << as_str << std::endl;
					}
				}
			}

		}

	}

	const unsigned int state::EvaluateExpression(const unsigned int max)
	{
		unsigned int count = 0;

		while (m_TopToken && m_TopToken->Type != tokenType::LineEnd && m_TopToken->Type != tokenType::ExpressionEnd && count < max)
		{
			if (m_TopToken->Type == tokenType::Identifier)
			{
				std::optional<const unsigned int> location = TraceIdentifier();
				if(location.has_value())
				{
					m_TopToken = m_TopToken->Next;
					if (m_TopToken->Type == tokenType::ExpressionStart)
					{
						// call function
					}
					else
					{
						m_ReturnStack.push(m_VarStack[location.value()]);
						++count;
					}
				}
			}

			else if (m_TopToken->Type == tokenType::StaticInt)
			{
				m_ReturnStack.push(new bisInt(std::stoi(m_Constants[m_TopToken->ID.value()]), false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticLong)
			{
				m_ReturnStack.push(new bisLong(std::stol(m_Constants[m_TopToken->ID.value()]), false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticFloat)
			{
				m_ReturnStack.push(new bisFloat(std::stof(m_Constants[m_TopToken->ID.value()]), false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticDouble)
			{
				m_ReturnStack.push(new bisDouble(std::stod(m_Constants[m_TopToken->ID.value()]), false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticByte)
			{
				m_ReturnStack.push(new bisByte(m_Constants[m_TopToken->ID.value()][0], false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticBoolTrue)
			{
				m_ReturnStack.push(new bisBool(true, false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticBoolFalse)
			{
				m_ReturnStack.push(new bisBool(false, false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticString)
			{
				m_ReturnStack.push(new bisString(m_Constants[m_TopToken->ID.value()], false));
				++count;
			}
			else if (m_TopToken->Type = tokenType::StaticNull)
			{
				m_ReturnStack.push(nullptr);
				++count;
			}


			m_TopToken = m_TopToken->Next;
		}

		return count;

	}

}