#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iLight
	{	
	public:
		iLight() {};
		virtual ~iLight() {};

		virtual void SetColor(Vector3 col) = 0;
		virtual Vector3 GetColor() const = 0;
		virtual void SetPosition(Vector3 pos) = 0;
		virtual Vector3 GetPosition() const = 0;
		virtual void Move(Vector3 moveBy) = 0;
		virtual void SetLookAt(Vector3 la) = 0;
		virtual void SetUp(Vector3 up) = 0;

		virtual void SetIntensity(float intensity) = 0;
		virtual float GetIntensity() const = 0;

		virtual bool IsUsingShadowMap() const = 0;
	};
}