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
	protected:
		iImage() {};
		virtual ~iImage() {};

	public:

		virtual Vector2 GetPosition() const = 0;
		virtual void SetPosition(Vector2 pos) = 0;

		/*
		Sets the strata (the rendering order) of the Image. By default 500 (0.5f, the middle). Higher strata = further away = will be behind other images that are closer.
		Internally 0.0f - 1.0f resolution is used, but when setting and getting the value I devide / multiply by 1000, to make the range 0.0f - 1000.0f for outside usage.
		Engine uses 10.0f for splash screens / loading screens.
		*/
		virtual void SetStrata(float strata) = 0;
		virtual float GetStrata() const = 0;
		virtual Vector2 GetDimensions() const  = 0;
		virtual void SetDimensions(Vector2 dims) = 0;
		virtual float GetOpacity() const = 0;
		virtual void SetOpacity(float opacity) = 0;
	};
}