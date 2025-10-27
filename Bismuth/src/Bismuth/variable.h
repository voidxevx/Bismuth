#pragma once

#include "Core.h"

#include <functional>
#include <string>
#include <memory>
#include <xhash>

namespace bis
{

	extern BISMUTH_API std::hash<std::string> e_IdentityHasher;

	namespace ValueType
	{
		typedef BISMUTH_API unsigned short ValueType;
		enum BISMUTH_API typeHint : uint8_t
		{
			Int,
			Uint,
			Long,
			Ulong,
			Float,
			Double,
			Byte,
			Ubyte,
			Bool,

			String,
			Null,
			Class,
		};

		enum BISMUTH_API typeModifier : uint8_t
		{
			Value,
			Pointer,
		};

		extern BISMUTH_API ValueType __ValueType(typeHint, typeModifier);

		struct BISMUTH_API ValueType_Val
		{
			union
			{
				typeHint hint;
				typeModifier mod;
			};
		};

		extern BISMUTH_API ValueType_Val __GetHint(ValueType);
		extern BISMUTH_API ValueType_Val __GetMod(ValueType);
	}

	extern BISMUTH_API bool is_int(const std::string&);
	extern BISMUTH_API bool is_long(const std::string&);
	extern BISMUTH_API bool is_float(const std::string&);
	extern BISMUTH_API bool is_double(const std::string&);



	template<typename _T> BISMUTH_API ValueType::ValueType GetHintType();
	extern template BISMUTH_API ValueType::ValueType GetHintType<int>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<unsigned int>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<long>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<unsigned long>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<float>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<double>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<char>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<unsigned char>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<bool>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<std::string>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<int*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<unsigned int*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<long*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<unsigned long*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<float*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<double*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<char*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<unsigned char*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<bool*>();
	extern template BISMUTH_API ValueType::ValueType GetHintType<std::string*>();
	

	class BISMUTH_API __IVariable
	{
	public:
		virtual ~__IVariable() = default;
		inline const ValueType::ValueType GetType() const { return m_Type; }
	protected:
		__IVariable(ValueType::ValueType type)
			: m_Type(type)
		{}
	private:
		const ValueType::ValueType m_Type;
	};


	template<typename _T>
	class BISMUTH_API __Variable : public __IVariable
	{
	public:
		__Variable(_T value)
			: __IVariable(GetHintType<_T>())
			, m_Value(value)
		{}

		virtual ~__Variable() = default;

		inline operator _T() { return m_Value; }
		inline _T Get() const { return m_Value; }
		inline _T& GetRef() { return m_Value; }
		inline _T* GetPtr() { return &m_Value; }
		inline void Set(_T newVal) { m_Value = newVal; }
		inline void operator=(_T newVal) { m_Value = newVal; }

	private:
		_T m_Value;
	};


	template<typename _T> BISMUTH_API _T __SmartCast(__IVariable*);
	extern template BISMUTH_API int __SmartCast<int>(__IVariable*);
	extern template BISMUTH_API unsigned int __SmartCast<unsigned int>(__IVariable*);
	extern template BISMUTH_API long __SmartCast<long>(__IVariable*);
	extern template BISMUTH_API unsigned long __SmartCast<unsigned long>(__IVariable*);
	extern template BISMUTH_API float __SmartCast<float>(__IVariable*);
	extern template BISMUTH_API double __SmartCast<double>(__IVariable*);
	extern template BISMUTH_API char __SmartCast<char>(__IVariable*);
	extern template BISMUTH_API unsigned char __SmartCast<unsigned char>(__IVariable*);
	extern template BISMUTH_API bool __SmartCast<bool>(__IVariable*);
	extern template BISMUTH_API std::string __SmartCast<std::string>(__IVariable*);

	template class BISMUTH_API __Variable<int>;
	template class BISMUTH_API __Variable<unsigned int>;
	template class BISMUTH_API __Variable<long>;
	template class BISMUTH_API __Variable<unsigned long>;
	template class BISMUTH_API __Variable<float>;
	template class BISMUTH_API __Variable<double>;
	template class BISMUTH_API __Variable<char>;
	template class BISMUTH_API __Variable<unsigned char>;
	template class BISMUTH_API __Variable<bool>;
	template class BISMUTH_API __Variable<std::string>;

	template class BISMUTH_API __Variable<int*>;
	template class BISMUTH_API __Variable<unsigned int*>;
	template class BISMUTH_API __Variable<long*>;
	template class BISMUTH_API __Variable<unsigned long*>;
	template class BISMUTH_API __Variable<float*>;
	template class BISMUTH_API __Variable<double*>;
	template class BISMUTH_API __Variable<char*>;
	template class BISMUTH_API __Variable<unsigned char*>;
	template class BISMUTH_API __Variable<bool*>;
	template class BISMUTH_API __Variable<std::string*>;




	typedef __Variable<int> bisInt;
	typedef __Variable<unsigned int> bisUint;
	typedef __Variable<long> bisLong;
	typedef __Variable<unsigned long> bisUlong;
	typedef __Variable<float> bisFloat;
	typedef __Variable<double> bisDouble;
	typedef __Variable<char> bisByte;
	typedef __Variable<unsigned char> bisUbyte;
	typedef __Variable<bool> bisBool;
	typedef __Variable<std::string> bisString;
	typedef __Variable<int*> bisInt_ptr;
	typedef __Variable<unsigned int*> bisUint_ptr;
	typedef __Variable<long*> bisLong_ptr;
	typedef __Variable<unsigned long*> bisUlong_ptr;
	typedef __Variable<float*> bisFloat_ptr;
	typedef __Variable<double*> bisDouble_ptr;
	typedef __Variable<char*> bisByte_ptr;
	typedef __Variable<unsigned char*> bisUbyte_ptr;
	typedef __Variable<bool*> bisBool_ptr;
	typedef __Variable<std::string*> bisString_ptr;
}