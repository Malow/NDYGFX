#pragma once

enum ANY_TYPE_TYPES
{
	ANY_TYPE_NONE,
	ANY_TYPE_FLOAT,
	ANY_TYPE_SINT,
	ANY_TYPE_UINT
};

struct AnyType
{
	union VALUEUNION
	{
		float f;
		unsigned int uint;
		signed int sint;
	} value;

	ANY_TYPE_TYPES curType;

	AnyType() : 
		curType(ANY_TYPE_NONE)
	{

	}

	AnyType(const float& f) : 
		curType(ANY_TYPE_FLOAT)
	{
		value.f = f;
	}

	AnyType(const unsigned int& uint) : 
		curType(ANY_TYPE_UINT)
	{
		value.uint = uint;
	}

	AnyType(const signed int& sint) : 
		curType(ANY_TYPE_SINT)
	{
		value.sint = sint;
	}

	inline AnyType& operator=(const float& fVal)
	{
		value.f = fVal;
		curType = ANY_TYPE_FLOAT;
		return *this;
	}

	inline AnyType& operator=(const unsigned int& uiVal)
	{
		value.uint = uiVal;
		curType = ANY_TYPE_UINT;
		return *this;
	}

	inline AnyType& operator=(const signed int& iVal)
	{
		value.sint = iVal;
		curType = ANY_TYPE_SINT;
		return *this;
	}

	inline operator float() const
	{
		if ( curType != ANY_TYPE_FLOAT ) throw("Wrong Type");
		return value.f;
	}

	inline operator unsigned int() const
	{
		if ( curType != ANY_TYPE_UINT ) throw("Wrong Type");
		return value.uint;
	}

	inline operator signed int() const
	{
		if ( curType != ANY_TYPE_SINT ) throw("Wrong Type");
		return value.sint;
	}

	inline bool operator==(const AnyType& val) const
	{
		if ( curType != val.curType )
		{
			return false;
		}

		if ( curType == ANY_TYPE_FLOAT ) return value.f == val.value.f;
		if ( curType == ANY_TYPE_SINT ) return value.sint == val.value.sint;
		if ( curType == ANY_TYPE_UINT ) return value.uint == val.value.uint;

		return false;
	}
};