#pragma once

#include "Core.h"

#include "DataTypes/token.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>

namespace bismuth
{
	template<typename _T>
	static void Delete_ptr(_T& ptr)
	{}
	template<typename _T>
	static void Delete_ptr(_T* ptr)
	{
		delete ptr;
		ptr = nullptr;
	}

	enum ValueType
	{
		Int,
		String,
		Float,
		Uint,
		Analogous,
		Custom,
		Nil,
	};

	class IBismuthVariable
	{
	public:
		IBismuthVariable(const std::string& name)
			: m_Name(name)
		{ }
		virtual ~IBismuthVariable() = default;
		
		inline const std::string& GetName() const { return m_Name; }
		virtual inline const std::string GetStaticType() const { return "NULL"; }
		virtual inline const void* const GetAnalogous() const { return nullptr; }
	protected:
		std::string m_Name;
	};

	template<typename _T>
	class BismuthVariable : public IBismuthVariable
	{
	public:
		BismuthVariable(const std::string& name, _T value)
			: IBismuthVariable(name)
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


		virtual inline const std::string GetStaticType() const override { return typeid(_T).name(); }
		virtual inline const void* const GetAnalogous() const override { return &m_Value; }

	private:
		_T m_Value;
	};


	template class BISMUTH_API BismuthVariable<int>;
	typedef        BISMUTH_API BismuthVariable<int>           bis_int;
	template class BISMUTH_API BismuthVariable<unsigned int>; 
	typedef        BISMUTH_API BismuthVariable<unsigned int>  bis_uint;
	template class BISMUTH_API BismuthVariable<float>;		  
	typedef        BISMUTH_API BismuthVariable<float>         bis_float;
	template class BISMUTH_API BismuthVariable<std::string>;  
	typedef        BISMUTH_API BismuthVariable<std::string>   bis_string;
	template class BISMUTH_API BismuthVariable<const char*>;  
	typedef        BISMUTH_API BismuthVariable<const char*>   bis_cstr;
	template class BISMUTH_API BismuthVariable<const void*>;  
	typedef        BISMUTH_API BismuthVariable<const void*>   bis_voidptr;

	#define bsint      bis_int
	#define bsuint     bis_uint
	#define bsfloat    bis_float
	#define bsstr      bis_string
	#define bscstr     bis_cstr
	#define bsvoidptr  bis_voidptr






	struct BISMUTH_API BismuthClassProperty
	{
		ValueType Type;
		unsigned int Offset;

		BismuthClassProperty()
			: Type(ValueType::Nil)
			, Offset(0)
		{}

		BismuthClassProperty(ValueType type, unsigned int offset)
			: Type(type)
			, Offset(offset)
		{}
	};

	class BISMUTH_API BismuthClassTemplate
	{
		friend class BismuthClass;
	public:
		BismuthClassTemplate(const std::string& className, const std::vector<std::pair<std::string, ValueType>>& properties, const std::shared_ptr<BismuthClassTemplate> parent = nullptr);

		const std::optional<BismuthClassProperty> getStaticProperty(const std::string& name) const;

	private:
		const std::string m_Name;
		const unsigned int m_TotalProperties;
		std::unordered_map<std::string, BismuthClassProperty> m_Properties;
		std::shared_ptr<BismuthClassTemplate> m_ParentClass;
	};

	class BISMUTH_API BismuthClass
	{
	public:
		BismuthClass(const std::shared_ptr<BismuthClassTemplate> _template);

		template<typename _T> 
		const std::optional<_T> getProperty(const std::string& name) const;

		template<typename _T>
		void setProperty(const std::string& name, const _T* const newValue);
		
	private:
		std::shared_ptr<BismuthClassTemplate> m_ClassTemplate;
		const void** m_Properties;
	};








	constexpr int STACK_MAX_SIZE = 128;

	class BISMUTH_API state
	{
	public:
		state();
		~state();

		std::vector<token> tokenize(const std::string& source);

		void BreakToken(const std::string& str, std::vector<std::string>& tokens) const;

		template<typename _T>
		void PushVariable(const std::string& name, _T value);

		template<typename _T>
		BismuthVariable<_T>& GetVariable(const std::string& name);

		void DoString(const std::string& source);

		inline void JoinThread() { m_OperationThread.join(); }

	private:
		void Evaluate(const std::vector<token>& tokens);
		void EvaluateExpression(const std::vector<token>& tokens, unsigned int& i);
		bool is_num(const std::string& str) const;

		void pushToReturn(const void* const ptr, bool dealloc = false);
		template<typename _T>
		_T const popFromReturn();

		std::unordered_map<std::string, IBismuthVariable*> m_Variables;
		const void** m_ReturnStack;
		bool* m_ReturnDeallocations;
		unsigned int m_ReturnsStackTop = 0;

		std::thread m_OperationThread;
	};

	template BISMUTH_API void state::PushVariable<int>(const std::string& name, int value);
	template BISMUTH_API void state::PushVariable<unsigned int>(const std::string& name, unsigned int value);
	template BISMUTH_API void state::PushVariable<float>(const std::string& name, float value);
	template BISMUTH_API void state::PushVariable<std::string>(const std::string& name, std::string value);
	template BISMUTH_API void state::PushVariable<const char*>(const std::string& name, const char* value);
	template BISMUTH_API void state::PushVariable<const void*>(const std::string& name, const void* value);

	template BISMUTH_API bis_int& state::GetVariable<int>(const std::string& name);
	template BISMUTH_API bis_uint& state::GetVariable<unsigned int>(const std::string& name);
	template BISMUTH_API bis_float& state::GetVariable<float>(const std::string& name);
	template BISMUTH_API bis_string& state::GetVariable<std::string>(const std::string& name);
	template BISMUTH_API bis_cstr& state::GetVariable<const char*>(const std::string& name);
	template BISMUTH_API bis_voidptr& state::GetVariable<const void*>(const std::string& name);

}