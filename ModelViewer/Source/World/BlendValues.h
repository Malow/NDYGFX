#pragma once

// Typical Defitions
#pragma push_macro("min")
#pragma push_macro("max")

#undef min
#undef max

#include <array>

template<unsigned int T>
struct BlendValuesT : public std::array<float, T>
{
public:
	BlendValuesT()
	{
		for( unsigned int x=0; x<T; ++x )
		{
			(*this)[x] = 0.0f;
		}
	}

	void Saturate()
	{
		float largest = std::numeric_limits<float>::min();
		for( unsigned int x=0; x<T; ++x )
		{
			if ( largest < (*this)[x] )
			{
				largest = (*this)[x];
			}
		}

		// Divide By Largest
		for( unsigned int x=0; x<T; ++x )
		{
			(*this)[x] /= largest;
		}
	}

	void Normalize()
	{
		float min = std::numeric_limits<float>::max();
		float max = 0.0f;

		for( unsigned int x=0; x<T; ++x )
		{
			if ( (*this)[x] < min ) min = (*this)[x];
			max += (*this)[x];
		}

		for( unsigned int x=0; x<T; ++x )
		{
			(*this)[x] = ((*this)[x] - min) / (max - min);
		}
	}

	BlendValuesT<T> operator+( const BlendValuesT<T>& val )
	{
		BlendValuesT<T> newVals;
		for( unsigned int x=0; x<T; ++x )
		{
			newVals[x] = (*this)[x] + val[x];
		}
		return newVals;
	}
};

#pragma pop_macro("min")
#pragma pop_macro("max")