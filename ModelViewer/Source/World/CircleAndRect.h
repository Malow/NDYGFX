#pragma once

#include <Vector.h>

// 2D Intersection Checks, By Alexivan

struct Circle
{
public:
	Vector2 center;
	float radius;

	Circle( const Vector2& center, float radius ) : 
		center(center),
		radius(radius)
	{
	}

	inline bool IsInside( const Vector2& point ) const
	{
		return Vector2(point.x-center.x,point.y-center.y).GetLength() <= radius;
	}
};


struct Rect
{
public:
	Vector2 topLeft;
	Vector2 size;	// x = width, y = height

	Rect( const Vector2& topLeft=Vector2(0.0f, 0.0f), const Vector2& size=Vector2(0.0f, 0.0f) ) :
		topLeft(topLeft),
		size(size)
	{
	}

	bool IsInside( const Vector2& point ) const
	{
		if ( point.x < topLeft.x ) return false;
		if ( point.y < topLeft.y ) return false;
		if ( point.x >= topLeft.x + size.x ) return false;
		if ( point.y >= topLeft.y + size.y ) return false;
		return true;
	}
};


inline bool DoesIntersect( const Circle& A, const Circle& B )
{
	return Vector2(A.center.x - B.center.x, A.center.y - B.center.y).GetLength() < A.radius + B.radius;
}


inline bool DoesIntersect( const Rect& A, const Rect& B )
{
	 if ( A.topLeft.x > B.topLeft.x + B.size.x ) return false;
	 if ( A.topLeft.y > B.topLeft.y + B.size.y ) return false;
	 if ( A.topLeft.x + A.size.x < B.topLeft.x ) return false;
	 if ( A.topLeft.y + A.size.y < B.topLeft.y ) return false;
	 return true;
}


inline bool DoesIntersect( const Rect& A, const Circle& B )
{
	float cornerRadius = Vector2(A.size.x/2.0f,A.size.y/2.0f).GetLength();
	if ( !DoesIntersect( Circle(Vector2(A.topLeft.x+A.size.x/2.0f,A.topLeft.y+A.size.y/2.0f), cornerRadius), B) )
		return false;

	if ( B.center.x + B.radius < A.topLeft.x ) return false;
	if ( B.center.x - B.radius > A.topLeft.x + A.size.x ) return false;
	if ( B.center.y + B.radius < A.topLeft.y ) return false;
	if ( B.center.y - B.radius > A.topLeft.y + A.size.y ) return false;
	
	return true;
}