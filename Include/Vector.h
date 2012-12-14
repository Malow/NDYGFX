#ifndef MALOWVECTOR
#define MALOWVECTOR

#include <math.h>

// Edit 2012-11-18 by Alexivan - Removed DX dependencies
// Edit 2012-11-23 by Alexivan - Added DX Conversions

class Vector2
{
public:
	float x;
	float y;

	Vector2(float x=0.0f, float y=0.0f) :
		x(x),
		y(y)
	{
	}


	inline float GetLength() const
	{
		return sqrt(pow(this->x, 2) + pow(this->y, 2));
	}


	inline void normalize()
	{
		float length = this->GetLength();
		if(length > 0.0f)
		{
			this->x /= length;
			this->y /= length;
		}
	}


	inline float GetDotProduct(const Vector2& compObj) const
	{
		float dot = this->x * compObj.x;
		dot += this->y * compObj.y;
		return dot;
	}


	inline Vector2 operator+(const Vector2& v) const
	{
		return Vector2(this->x+v.x, this->y+v.y);
	}


	inline Vector2 operator-(const Vector2& v) const
	{
		return Vector2(this->x-v.x, this->y-v.y);
	}


	inline Vector2 operator*(const float& scalar) const
	{
		return Vector2(this->x*scalar, this->y*scalar);
	}


	inline Vector2 operator/(const float& scalar) const
	{
		return Vector2(this->x/scalar, this->y/scalar);
	}


	inline void operator+=(const Vector2& v)
	{
		x += v.x;
		y += v.y;
	}


	inline void operator-=(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
	}


	inline void operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
	}


	inline float GetLengthSquared() const
	{
		return this->GetDotProduct(*this);
	}

	
	inline bool operator==( const Vector2& v ) const
	{
		return ( x == v.x && y == v.y );
	}


	inline bool operator!=( const Vector2& v ) const
	{
		return ( x != v.x || y != v.y );
	}
};


class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3(float x=0.0f, float y=0.0f, float z=0.0f) :
		x(x),
		y(y),
		z(z)
	{
	}


	Vector3( const Vector2& v, float z ) :
		x(v.x),
		y(v.y),
		z(z)
	{

	}


	inline float GetLength()
	{
		return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
	}


	inline void normalize()
	{
		float length = this->GetLength();
			
		if(length > 0.0f)
		{
			this->x /= length;
			this->y /= length;
			this->z /= length;
		}
	}


	inline float GetDotProduct(Vector3& compObj)
	{
		float dot = this->x * compObj.x;
		dot += this->y * compObj.y;
		dot += this->z * compObj.z;
		return dot;
	}


	inline Vector3 GetCrossProduct(Vector3 vec)
	{
		Vector3 retVec;
		retVec.x = this->y * vec.z - vec.y * this->z;
		retVec.y = -(this->x * vec.z - vec.x * this->z);
		retVec.z = this->x * vec.y - vec.x * this->y;

		return retVec;
	}


	inline float GetAngle(Vector3& compObj)
	{
		return acos(this->GetDotProduct(compObj) / (this->GetLength() * compObj.GetLength()));
	}

		
	inline Vector3 operator+(const Vector3& v) const
    {
        return Vector3(this->x+v.x, this->y+v.y, this->z+v.z);
    }


	inline Vector3 operator-(const Vector3& v) const
	{
		return Vector3(this->x-v.x, this->y-v.y, this->z-v.z);
	}


	inline Vector3 operator*(const float& scalar) const
	{
		return Vector3(this->x*scalar, this->y*scalar, this->z*scalar);
	}


	inline Vector3 operator/(const float& scalar) const
	{
		return Vector3(this->x/scalar, this->y/scalar, this->z/scalar);
	}


	inline void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }


	inline void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }


	inline void operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }


	inline float GetLengthSquared()
	{
		return this->GetDotProduct(*this);
	}


	inline Vector3 GetComponentMultiplication(const Vector3 & compVec)
	{
		return Vector3(this->x*compVec.x, this->y*compVec.y, this->z*compVec.z);
	}


	inline void RotateY(float angle)
	{
		Vector3 vec = *this;
		vec.x = cos(angle) * this->x + sin(angle) * this->z;
		vec.z = -sin(angle) * this->x + cos(angle) * this->z;
		*this = vec;
	}


	inline Vector3 GetRoteted(float angle)
	{
		Vector3 vec = *this;
		vec.RotateY(angle);
		return vec;
	}


	inline Vector3 GetInverseComponents()
	{
		return Vector3(1.0f/this->x, 1.0f/this->y, 1.0f/this->z);
	}


	inline bool operator==( const Vector3& v ) const
	{
		return ( x == v.x && y == v.y && z == v.z );
	}


	inline bool operator!=( const Vector3& v ) const
	{
		return ( x != v.x || y != v.y || z != v.z );
	}


#ifdef D3DVECTOR_DEFINED
	operator D3DXVECTOR3 () const { return D3DXVECTOR3(x,y,z); }
#endif
};


class Vector4
{
public:
	float x,y,z,w;

	Vector4(float x=0.0f, float y=0.0f, float z=0.0f, float w=0.0f) : 
		x(x), 
		y(y), 
		z(z), 
		w(w)
	{
	}

	Vector4( const Vector3& v, float w=0.0f ) :
		x(v.x),
		y(v.y),
		z(v.z),
		w(w)
	{
	}


	inline bool operator==( const Vector4& v ) const
	{
		return ( x == v.x && y == v.y && z == v.z && w == v.w );
	}


	inline bool operator!=( const Vector4& v ) const
	{
		return ( x != v.x || y != v.y || z != v.z || w != v.w );
	}

#ifdef D3DVECTOR_DEFINED
	operator D3DXVECTOR4 () const { return D3DXVECTOR4(x,y,z,w); }
#endif
};


#endif