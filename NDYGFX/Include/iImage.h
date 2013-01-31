#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iImage
	{	
	public:
		iImage() {};
		virtual ~iImage() {};

		virtual Vector2 GetPosition() const = 0;
		virtual void SetPosition(Vector2 pos) = 0;
		virtual Vector2 GetDimensions() const  = 0;
		virtual void SetDimensions(Vector2 dims) = 0;
		virtual float GetOpacity() const = 0;
		virtual void SetOpacity(float opacity) = 0;
	};
}