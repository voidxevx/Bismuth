#pragma once

/* Rex Bradbury, 10/22/25 - Bismuth
   Apothicary runtime scripting language

	TODO: Trace Identifiers - Get pointer to class property

*/

#include "Core.h"

#include <optional>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <functional>

namespace bismuth
{
	template<typename _T>
	static void Delete_ptr(_T& ptr)
	{}
	template<typename _T>
	static void Delete_ptr(_T* ptr)
	{
		if (ptr)
			delete ptr;
		ptr = nullptr;
	}

	enum BISMUTH_API tokenType
	{
		Identifier,
		Assignment, // =
		LineEnd, // ;
		ExpressionBreak, // ,
		ExpressionStart, // (
		ExpressionEnd, // )
		ScopeStart, // {
		ScopeEnd, // }
		Property, // :

		StaticInt,
		StaticFloat,
		StaticString,
		StaticNil,
		StaticBool_True, // true
		StaticBool_False, // false

		Return, // return
		ClassDecl, // class
		Inherite, // extends
		PublicDomain, // public
		ProtectedDomain, // protected
		PrivateDomain, // private
	};

	struct BISMUTH_API token
	{
		tokenType Type;
		std::optional<std::string> Value;

		token(tokenType type)
			: Type(type)
		{
		}
		token(tokenType type, std::string val)
			: Type(type)
			, Value(val)
		{
		}
	};






	enum ValueType
	{
		Int,
		String,
		Float,
		Uint,
		Bool,
		Analogous,
		Custom,
		Nil,
	};

	class IBismuthVariable
	{
	public:
		IBismuthVariable(ValueType type)
			: m_Type(type)
		{}
		virtual ~IBismuthVariable() = default;
		
		inline const ValueType GetStaticType() const { return m_Type; }
		virtual inline const void* const GetAnalogous() const { return nullptr; }

	protected:
		ValueType m_Type;
	};

	template<typename _T>
	class BismuthVariable : public IBismuthVariable
	{
	public:
		BismuthVariable(ValueType type, _T value)
			: IBismuthVariable(type)
			, m_Value(value)
		{}

		virtual ~BismuthVariable()
		{
			Delete_ptr(m_Value);
		}

		inline const _T Get() const { return m_Value; }
		inline _T& Get() { return m_Value; }
		inline void Set(_T newVal) { m_Value = newVal; }

		inline operator _T() const { return m_Value; }

		template<typename _Ty>
		inline const _Ty Cast() const { return static_cast<_Ty>(_T(*this)); }
		template<typename _Ty>
		inline const _Ty Cast_ptr() const { return *static_cast<const _Ty*>(_T(*this)); }


		virtual inline const void* const GetAnalogous() const override { return &m_Value; }

	private:
		_T m_Value;
	};


	template class BISMUTH_API BismuthVariable<int>;
	typedef        BISMUTH_API BismuthVariable<int>           bis_int;
	template class BISMUTH_API BismuthVariable<unsigned int>; 
	typedef        BISMUTH_API BismuthVariable<unsigned int>  bis_uint;
	template class BISMUTH_API BismuthVariable<bool>; 
	typedef        BISMUTH_API BismuthVariable<bool>          bis_bool;
	template class BISMUTH_API BismuthVariable<float>;		  
	typedef        BISMUTH_API BismuthVariable<float>         bis_float;
	template class BISMUTH_API BismuthVariable<std::string>;  
	typedef        BISMUTH_API BismuthVariable<std::string>   bis_string;
	template class BISMUTH_API BismuthVariable<const void*>;  
	typedef        BISMUTH_API BismuthVariable<const void*>   bis_voidptr;

	





	struct BismuthFunctionInput
	{
		ValueType Type;
		std::string Name;
		std::optional<std::string> CustomType;

		BismuthFunctionInput()
			: Type(ValueType::Nil)
			, Name("NULL")
			, CustomType(std::nullopt)
		{}

		BismuthFunctionInput(ValueType type, const std::string& name, std::optional<std::string> customType)
			: Type(type)
			, Name(name)
			, CustomType(customType)
		{}

	};


	enum BismuthFunctionRelativity
	{
		Native,
		Local,
		NONE,
	};

	class BISMUTH_API IBismuthFunction
	{
		friend class state;
	public:
		virtual ~IBismuthFunction() = default;

		virtual inline BismuthFunctionRelativity GetRelativity() const { return m_Relativity; }

	protected:
		IBismuthFunction(BismuthFunctionRelativity rel, ValueType returnType, std::vector<BismuthFunctionInput> inputs, std::optional<std::string> retCustom)
			: m_Relativity(rel)
			, m_FunctionReturnType(returnType)
			, m_FunctionInputs(inputs)
			, m_FunctionReturnCustom(retCustom)
		{}
	private:

		BismuthFunctionRelativity m_Relativity;

		ValueType m_FunctionReturnType;
		std::optional<std::string> m_FunctionReturnCustom;

		std::vector<BismuthFunctionInput> m_FunctionInputs;
	};

	class BISMUTH_API BismuthFunction_Local : public IBismuthFunction
	{
		friend class state;
	public:
		BismuthFunction_Local(ValueType returnType, std::vector<BismuthFunctionInput> inputs, const std::vector<token> tokens, std::optional<std::string> retCustom = std::nullopt)
			: IBismuthFunction(BismuthFunctionRelativity::Local, returnType, inputs, retCustom)
			, m_Tokens(tokens)
		{}
		virtual ~BismuthFunction_Local() = default;

	private:
		std::vector<token> m_Tokens;
	};

	



	enum BismuthClassDomain
	{
		Public,
		Protected,
		Private,
	};

	struct BISMUTH_API BismuthClassProperty
	{
		ValueType Type;
		unsigned int Offset;
		BismuthClassDomain Domain;

		BismuthClassProperty()
			: Type(ValueType::Nil)
			, Offset(0)
			, Domain(BismuthClassDomain::Public)
		{}

		BismuthClassProperty(ValueType type, unsigned int offset, BismuthClassDomain domain)
			: Type(type)
			, Offset(offset)
			, Domain(domain)
		{}
	};

	struct PropertyTemplate
	{
		ValueType type;
		BismuthClassDomain domain;
		std::string Name;

		PropertyTemplate()
			: type(ValueType::Nil)
			, domain(BismuthClassDomain::Public)
			, Name("")
		{}

		PropertyTemplate(ValueType _type, BismuthClassDomain _domain, const std::string& name)
			: type(_type)
			, domain(_domain)
			, Name(name)
		{}
	};

	class BISMUTH_API BismuthClassTemplate
	{
		friend class BismuthClassInstance;
	public:
		BismuthClassTemplate(const std::string& name, const std::vector<PropertyTemplate>& properties, std::map<std::string, IBismuthFunction*> functions, const std::shared_ptr<BismuthClassTemplate> parent = nullptr);

		const std::optional<BismuthClassProperty> getStaticProperty(const std::string& name, bool local) const;

		static std::shared_ptr<BismuthClassTemplate> CreateClassTemplate(const std::string& name, const std::vector<PropertyTemplate>& properties, std::map<std::string, IBismuthFunction*> functions, const std::shared_ptr<BismuthClassTemplate> parent = nullptr);

		inline const std::string GetName() const { return m_ClassName; }

	private:
		const unsigned int m_TotalProperties;
		std::map<std::string, BismuthClassProperty> m_Properties;
		std::map<std::string, IBismuthFunction*> m_Functions;
		std::shared_ptr<BismuthClassTemplate> m_ParentClass;
		const std::string m_ClassName;
	};

	typedef std::shared_ptr<BismuthClassTemplate> bis_class_template;


	enum BismuthClassInstanceRelativity
	{
		Templated,
		UserHandle,
	};

	class BISMUTH_API IBismuthClassInstance
	{
	public:
		virtual ~IBismuthClassInstance() = default;

		inline const BismuthClassInstanceRelativity GetStaticRelativity() const { return m_Relativity; }

		template<typename _T>
		static const std::optional<_T> GetInstanceProperty(const IBismuthClassInstance* const instance, const std::string& name);
		template<typename _T>
		static void SetInstanceProperty(IBismuthClassInstance* const instance, const std::string& name, _T newValue);

		virtual void* const GetPropertyPtr(const std::string&, bool) const { return nullptr; }

	protected:
		IBismuthClassInstance(BismuthClassInstanceRelativity relativity)
			: m_Relativity(relativity)
		{}
	private:
		BismuthClassInstanceRelativity m_Relativity;
	};

	template BISMUTH_API const std::optional<int> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const, const std::string&);
	template BISMUTH_API const std::optional<std::string> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const, const std::string&);
	template BISMUTH_API const std::optional<float> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const, const std::string&);
	template BISMUTH_API const std::optional<unsigned int> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const, const std::string&);
	template BISMUTH_API const std::optional<bool> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const, const std::string&);
	template BISMUTH_API const std::optional<const void*> IBismuthClassInstance::GetInstanceProperty(const IBismuthClassInstance* const, const std::string&);

	template BISMUTH_API void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const, const std::string&, int);
	template BISMUTH_API void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const, const std::string&, std::string);
	template BISMUTH_API void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const, const std::string&, float);
	template BISMUTH_API void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const, const std::string&, unsigned int);
	template BISMUTH_API void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const, const std::string&, bool);
	template BISMUTH_API void IBismuthClassInstance::SetInstanceProperty(IBismuthClassInstance* const, const std::string&, const void*);

	class BISMUTH_API BismuthClassInstance : public IBismuthClassInstance
	{
	public:
		BismuthClassInstance(const std::shared_ptr<BismuthClassTemplate> _template);
		virtual ~BismuthClassInstance() = default;

		template<typename _T> 
		const std::optional<_T> getProperty(const std::string& name) const;

		template<typename _T>
		void setProperty(const std::string& name, _T newValue);

		virtual void* const GetPropertyPtr(const std::string&, bool) const override;
		
	private:
		std::shared_ptr<BismuthClassTemplate> m_ClassTemplate;
		void** m_Properties;
	};

	
	struct BismuthUserHandle
	{
		ValueType Type;
		void* Target;

		BismuthUserHandle()
			: Type(ValueType::Nil)
			, Target(new int(0))
		{}

		BismuthUserHandle(ValueType type, void* target)
			: Type(type)
			, Target(target)
		{}
	};

	class BISMUTH_API BismuthClassInstance_UserHandle : public IBismuthClassInstance
	{
	public:
		BismuthClassInstance_UserHandle(const std::vector<std::pair<std::string, BismuthUserHandle>>& handles);
		virtual ~BismuthClassInstance_UserHandle() = default;

		template<typename _T>
		const std::optional<_T> getProperty(const std::string& name) const;

		template<typename _T>
		void setProperty(const std::string& name, _T newValue);

		virtual void* const GetPropertyPtr(const std::string&, bool) const override;

	private:
		std::map<std::string, BismuthUserHandle> m_Properties;
	};

	

	template class BISMUTH_API BismuthVariable<IBismuthClassInstance*>;
	typedef        BISMUTH_API BismuthVariable<IBismuthClassInstance*> bis_class;






	class VariableScope
	{
	public:
		VariableScope(IBismuthClassInstance* const scopeThis = nullptr)
			: m_This(scopeThis)
		{}


		void InsertVariable(const std::string& name, const unsigned int location)
		{
			++m_ScopeSize;
			m_VariableLocations[name] = location;
		}

		inline const unsigned int GetVariableLocation(const std::string& name) const { return m_VariableLocations.at(name); }
		inline const bool HasVariable(const std::string& name) const { return m_VariableLocations.count(name) > 0; }
		inline const unsigned int GetScopeSize() const { return m_ScopeSize; }
		inline IBismuthClassInstance* const& GetThis() { return m_This; }

	private:
		unsigned int m_ScopeSize = 0; // amount to deallocate when destroyed
		std::map<std::string, unsigned int> m_VariableLocations; // the locations of the variables
		IBismuthClassInstance* const m_This; // the scope access point
	};









	constexpr int STACK_MAX_SIZE = 128;

	class BISMUTH_API state
	{
	public:
		state();
		~state();

		std::vector<token> tokenize(const std::string& source);

		void PushScope();
		void PopScope();

		template<typename _T>
		void PushVariable(const std::string& name, ValueType type, _T value);
		void PushVariable(const std::string& name, IBismuthVariable* const var);

		template<typename _T>
		std::optional<BismuthVariable<_T>*> GetVariable(const std::string& name);
		std::optional<IBismuthVariable*> GetVariable(const std::string& name);

		template<typename _T>
		const _T var(const std::string& name) { return GetVariable<_T>(name).value()->Get(); }

		void pushToReturn(const void* const ptr, ValueType type, bool dealloc = false);

		void PushFunction(const std::string& name, IBismuthFunction* function);


		void Build(const std::vector<token>& tokens);
		void BuildString(const std::string& source);
		void DoString(const std::string& source);
		void Evaluate(const std::vector<token>& tokens);
		void EvaluateExpression(const std::vector<token>& tokens, unsigned int& i);
		void RunFunction(const IBismuthFunction* const function);

		inline void JoinThread() 
		{ m_OperationThread.join(); }
		inline void JoinFinishedBuild()
		{ m_BuildThread.join(); }

		template<typename _T>
		_T popFromReturn();

		static bool is_num_int(const std::string& str);
		static bool is_num_float(const std::string& str);

		enum TraceType
		{
			Function,
			Variable,
			Class,
			Property,
			NONE,
		};
		struct TraceObject
		{
			TraceType Type;
			void* ptr;

			TraceObject()
				: Type(TraceType::NONE)
				, ptr(nullptr)
			{}

			TraceObject(TraceType type, void* const _ptr)
				: Type(type)
				, ptr(_ptr)
			{}
		};

		TraceObject TraceForObjects(const std::vector<token>& tokens, unsigned int& i, const state::TraceObject& last = {});

	private:
		void BreakToken(const std::string& str, std::vector<std::string>& tokens) const;
		std::vector<BismuthFunctionInput> GetInputs(const std::vector<token>& tokens, unsigned int& i) const;


		std::map<std::string, IBismuthFunction*> m_GlobalFunctions;
		std::map<std::string, std::shared_ptr<BismuthClassTemplate>> m_ClassTypes;

		VariableScope** m_ScopeStack;
		unsigned int m_ScopeStackTop = 0;
		IBismuthVariable** m_VariableStack;
		unsigned int m_VariableStackTop = 0;

		struct ReturnValue
		{
			const void* Value;
			ValueType Type;
			bool deallocate;
		};
		ReturnValue* m_ReturnStack;
		unsigned int m_ReturnsStackTop = 0;

		std::thread m_OperationThread;
		std::thread m_BuildThread;
	};

	template BISMUTH_API void state::PushVariable<int>(const std::string&, ValueType, int);
	template BISMUTH_API void state::PushVariable<unsigned int>(const std::string&, ValueType, unsigned int);
	template BISMUTH_API void state::PushVariable<float>(const std::string&, ValueType, float);
	template BISMUTH_API void state::PushVariable<bool>(const std::string&, ValueType, bool);
	template BISMUTH_API void state::PushVariable<std::string>(const std::string&, ValueType, std::string);
	template BISMUTH_API void state::PushVariable<const void*>(const std::string&, ValueType, const void*);

	template BISMUTH_API std::optional<bis_int*> state::GetVariable<int>(const std::string&);
	template BISMUTH_API std::optional<bis_uint*> state::GetVariable<unsigned int>(const std::string&);
	template BISMUTH_API std::optional<bis_float*> state::GetVariable<float>(const std::string&);
	template BISMUTH_API std::optional<bis_bool*> state::GetVariable<bool>(const std::string&);
	template BISMUTH_API std::optional<bis_string*> state::GetVariable<std::string>(const std::string&);
	template BISMUTH_API std::optional<bis_voidptr*> state::GetVariable<const void*>(const std::string&);
	template BISMUTH_API std::optional<bis_class*> state::GetVariable<IBismuthClassInstance*>(const std::string&);



	class BISMUTH_API BismuthFunction_Native : public IBismuthFunction
	{
		friend class state;
		using bis_lambda = std::function<void(state* const)>;
	public:
		BismuthFunction_Native(ValueType returnType, std::vector<BismuthFunctionInput> inputs, bis_lambda func, std::optional<std::string> retCustom = std::nullopt)
			: IBismuthFunction(BismuthFunctionRelativity::Native, returnType, inputs, retCustom)
			, m_NativeFunction(func)
		{}

	private:
		bis_lambda m_NativeFunction;
	};


}