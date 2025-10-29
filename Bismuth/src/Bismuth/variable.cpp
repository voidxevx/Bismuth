#include "variable.h"

#include <stdexcept>

namespace bis
{

	std::hash<std::string> e_IdentityHasher = std::hash<std::string>();

	namespace ValueType
	{
		inline ValueType BISMUTH_API
		__ValueType(typeHint type, typeModifier mod)
		{
			return ValueType(((ValueType)type << 8) |  mod);
		}

		inline ValueType_Val BISMUTH_API
		__GetHint(ValueType type)
		{
			ValueType_Val r{};
			r.hint = static_cast<typeHint>((type >> 8) & 0xFF);
			return r;
		}

		inline ValueType_Val BISMUTH_API
		__GetMod(ValueType type)
		{
			ValueType_Val r{};
			r.mod = static_cast<typeModifier>(type & 0xFF);
			return r;
		}

		
	}



	inline BISMUTH_API bool
	is_int(const std::string& str)
	{
		try
		{
			int num = std::stoi(str);
			return true;
		}
		catch (const std::invalid_argument&)
		{
			return false;
		}
		catch (const std::out_of_range&)
		{
			return false;
		}
	}

	inline BISMUTH_API bool
	is_long(const std::string& str)
	{
		try
		{
			long num = std::stol(str);
			return true;
		}
		catch (const std::invalid_argument&)
		{
			return false;
		}
		catch (const std::out_of_range&)
		{
			return false;
		}
	}

	inline BISMUTH_API bool
	is_float(const std::string& str)
	{
		try
		{
			float num = std::stof(str);
			return true;
		}
		catch (const std::invalid_argument&)
		{
			return false;
		}
		catch (const std::out_of_range&)
		{
			return false;
		}
	}

	inline BISMUTH_API bool
	is_double(const std::string& str)
	{
		try
		{
			double num = std::stod(str);
			return true;
		}
		catch (const std::invalid_argument&)
		{
			return false;
		}
		catch (const std::out_of_range&)
		{
			return false;
		}
	}



	inline BISMUTH_API __IVariable*
	NullType(ValueType::ValueType type, bool _mutable)
	{
		ValueType::typeHint hint = ValueType::__GetHint(type).hint;
		switch (hint)
		{
		case ValueType::typeHint::Int:
			return new __Variable<int>(0, _mutable);
		case ValueType::typeHint::Uint:
			return new __Variable<unsigned int>(0, _mutable);
		case ValueType::typeHint::Long:
			return new __Variable<int>(0L, _mutable);
		case ValueType::typeHint::Ulong:
			return new __Variable<unsigned long>(0UL, _mutable);
		case ValueType::typeHint::Float:
			return new __Variable<float>(0.0f, _mutable);
		case ValueType::typeHint::Double:
			return new __Variable<double>(0.0, _mutable);
		case ValueType::typeHint::Byte:
			return new __Variable<char>(' ', _mutable);
		case ValueType::typeHint::Ubyte:
			return new __Variable<unsigned char>(' ', _mutable);
		case ValueType::typeHint::Bool:
			return new __Variable<bool>(false, _mutable);
		case ValueType::typeHint::String:
			return new __Variable<std::string>("", _mutable);
		}
	}





	template<typename _T> BISMUTH_API ValueType::ValueType 
	GetHintType()
	{
		return ValueType::__ValueType(ValueType::typeHint::Null, ValueType::typeModifier::Value, false);
	}

#define GETHINTTYPE(t, tt, tm) template<> BISMUTH_API ValueType::ValueType \
								  GetHintType<t>()\
								  { return ValueType::__ValueType(ValueType::typeHint::tt, ValueType::typeModifier::tm); }

	GETHINTTYPE(int, Int, Value)
	GETHINTTYPE(unsigned int, Uint, Value)
	GETHINTTYPE(long, Long, Value)
	GETHINTTYPE(unsigned long, Ulong, Value)
	GETHINTTYPE(float, Float, Value)
	GETHINTTYPE(double, Double, Value)
	GETHINTTYPE(char, Byte, Value)
	GETHINTTYPE(unsigned char, Ubyte, Value)
	GETHINTTYPE(bool, Bool, Value)
	GETHINTTYPE(std::string, String, Value)
	GETHINTTYPE(int*, Int, Pointer)
	GETHINTTYPE(unsigned int*, Uint, Pointer)
	GETHINTTYPE(long*, Long, Pointer)
	GETHINTTYPE(unsigned long*, Ulong, Pointer)
	GETHINTTYPE(float*, Float, Pointer)
	GETHINTTYPE(double*, Double, Pointer)
	GETHINTTYPE(char*, Byte, Pointer)
	GETHINTTYPE(unsigned char*, Ubyte, Pointer)
	GETHINTTYPE(bool*, Bool, Pointer)
	GETHINTTYPE(std::string*, String, Pointer)





	template<typename _T> BISMUTH_API _T
	__SmartCast(__IVariable* var)
	{
		return _T(dynamic_cast<__Variable<_T>*>(var));
	}

#define SMARTCAST(t) template<> BISMUTH_API t \
	 __SmartCast<t>(__IVariable* var)
#define GETHINT ValueType::typeHint hint = ValueType::__GetHint(var->GetType()).hint;
#define CASTVAR(t) __Variable<t>* var_##t = dynamic_cast<__Variable<t>*>(var)

	SMARTCAST(int)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (int)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return (int)var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return (int)var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return (int)var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return (int)var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (int)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (int)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (int)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const std::string as_str = var_str->Get();
				if (is_int(as_str))
					return std::stoi(as_str);
				else
					return 0;
			}
			case ValueType::typeHint::Null:
			{
				return 0;
			}
			}

	}

	SMARTCAST(unsigned int)
	{
		GETHINT
		switch (hint)
		{
		case ValueType::typeHint::Int:
		{
			CASTVAR(int);
			return (unsigned)var_int->Get();
		}
		case ValueType::typeHint::Uint:
		{
			using uint = unsigned int;
			CASTVAR(uint);
			return var_uint->Get();
		}
		case ValueType::typeHint::Long:
		{
			CASTVAR(long);
			return (unsigned int)var_long->Get();
		}
		case ValueType::typeHint::Ulong:
		{
			using ulong = unsigned long;
			CASTVAR(ulong);
			return (unsigned int)var_ulong->Get();
		}
		case ValueType::typeHint::Float:
		{
			CASTVAR(float);
			return (unsigned int)var_float->Get();
		}
		case ValueType::typeHint::Double:
		{
			CASTVAR(double);
			return (unsigned int)var_double->Get();
		}
		case ValueType::typeHint::Byte:
		{
			CASTVAR(char);
			return (unsigned int)var_char->Get();
		}
		case ValueType::typeHint::Ubyte:
		{
			using uchar = unsigned char;
			CASTVAR(uchar);
			return (unsigned int)var_uchar->Get();
		}
		case ValueType::typeHint::Bool:
		{
			CASTVAR(bool);
			return (int)var_bool->Get();
		}
		case ValueType::typeHint::String:
		{
			using str = std::string;
			CASTVAR(str);
			const std::string as_str = var_str->Get();
			if (is_int(as_str))
				return (unsigned int)std::stoi(as_str);
			else
				return 0U;
		}
		case ValueType::typeHint::Null:
		{
			return 0U;
		}
		}
	}

	SMARTCAST(long)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return (long)var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (long)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return (long)var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return (long)var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return (long)var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (long)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (long)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (long)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const str as_str = var_str->Get();
				if (is_long(as_str))
					return std::stol(as_str);
				else
					return 0L;
			}
			case ValueType::typeHint::Null:
			{
				return 0L;
			}
			}
	}

	SMARTCAST(unsigned long)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return (unsigned long)var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (unsigned long)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return (unsigned)var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return (unsigned long)var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return (unsigned long)var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (unsigned long)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (unsigned long)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (unsigned long)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const str as_str = var_str->Get();
				if (is_long(as_str))
					return (unsigned)std::stol(as_str);
				else
					return 0UL;
			}
			case ValueType::typeHint::Null:
			{
				return 0UL;
			}
			}
	}

	SMARTCAST(float)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return (float)var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (float)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return (float)var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return (float)var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return (float)var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (float)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (float)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (float)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const str as_str = var_str->Get();
				if (is_float(as_str))
					return std::stof(as_str);
				else
					return 0.0f;
			}
			case ValueType::typeHint::Null:
			{
				return 0.0f;
			}
			}
	}

	SMARTCAST(double)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return (double)var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (double)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return (double)var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return (double)var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return (double)var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (double)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (double)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (double)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const str as_str = var_str->Get();
				if (is_double(as_str))
					return std::stod(as_str);
				else
					return 0.0;
			}
			case ValueType::typeHint::Null:
			{
				return 0.0;
			}
			}
	}

	SMARTCAST(char)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return (char)var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (char)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return (char)var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return (char)var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return (char)var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return (char)var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (char)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (char)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const str as_str = var_str->Get();
				if (as_str.size() == 1)
					return as_str[0];
				else
					return ' ';
			}
			case ValueType::typeHint::Null:
			{
				return ' ';
			}
			}
	}

	SMARTCAST(unsigned char)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return (unsigned char)var_int->Get();
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return (unsigned char)var_uint->Get();
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return (unsigned char)var_long->Get();
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return (unsigned char)var_ulong->Get();
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return (unsigned char)var_float->Get();
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return (unsigned char)var_double->Get();
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (unsigned)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return (unsigned char)var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				const str as_str = var_str->Get();
				if (as_str.size() == 1)
					return as_str[0];
				else
					return ' ';
			}
			case ValueType::typeHint::Null:
			{
				return ' ';
			}
			}
	}

	SMARTCAST(bool)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return var_int->Get() != 0;
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return var_uint->Get() != 0;
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return var_long->Get() != 0;
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return var_ulong->Get() != 0;
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return var_float->Get() != 0;
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return var_double->Get() != 0;
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return (bool)var_char->Get();
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return (bool)var_uchar->Get();
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				return var_bool->Get();
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				return (var_str->Get() == "true");
			}
			case ValueType::typeHint::Null:
			{
			}
			}
	}

	SMARTCAST(std::string)
	{
		GETHINT
			switch (hint)
			{
			case ValueType::typeHint::Int:
			{
				CASTVAR(int);
				return std::to_string(var_int->Get());
			}
			case ValueType::typeHint::Uint:
			{
				using uint = unsigned int;
				CASTVAR(uint);
				return std::to_string(var_uint->Get());
			}
			case ValueType::typeHint::Long:
			{
				CASTVAR(long);
				return std::to_string(var_long->Get());
			}
			case ValueType::typeHint::Ulong:
			{
				using ulong = unsigned long;
				CASTVAR(ulong);
				return std::to_string(var_ulong->Get());
			}
			case ValueType::typeHint::Float:
			{
				CASTVAR(float);
				return std::to_string(var_float->Get());
			}
			case ValueType::typeHint::Double:
			{
				CASTVAR(double);
				return std::to_string(var_double->Get());
			}
			case ValueType::typeHint::Byte:
			{
				CASTVAR(char);
				return std::string(1, var_char->Get());
			}
			case ValueType::typeHint::Ubyte:
			{
				using uchar = unsigned char;
				CASTVAR(uchar);
				return std::string(1, var_uchar->Get());
			}
			case ValueType::typeHint::Bool:
			{
				CASTVAR(bool);
				if (var_bool)
					return "true";
				else
					return "false";
			}
			case ValueType::typeHint::String:
			{
				using str = std::string;
				CASTVAR(str);
				return var_str->Get();
			}
			case ValueType::typeHint::Null:
			{
				return "NULL";
			}
			}
	}
}