#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iDecal
	{	
	protected:
		virtual ~iDecal() {};

	public:
		iDecal() {};

		virtual float GetOpacity() const = 0;
		virtual void SetOpacity(float opacity) = 0;
		virtual float GetSize() const = 0;
		virtual void SetSize(float size) = 0;
		virtual Vector3 GetPosition() const = 0;
		virtual void SetPosition(Vector3 pos) = 0;
		virtual Vector3 GetDirection() = 0;
		virtual Vector3 GetUp() = 0;
	};
}