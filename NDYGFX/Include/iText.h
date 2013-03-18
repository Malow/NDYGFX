#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iText
	{	
	public:
		iText() {};
		virtual ~iText() {};

		/*
		Sets an overlay color over the text in range of 0 - 255 RGB. The internal value gets divided by 255
		so if you wanna get color it will be in the range of 0 - 1 for now. 
		Negative values can be used to subtract color from the original text as well.
		*/
		virtual void SetColor(Vector3 color) = 0;
		virtual void SetText(const char* text) = 0;
		virtual const char* GetText() const = 0;
		virtual void AppendText(const char* app) = 0;
		virtual void DeleteFromEnd(unsigned int CharsToDel) = 0;
		virtual void SetPosition(Vector2 pos)  = 0;
		virtual Vector2 GetPosition() const = 0;
		virtual void SetSize(float size) = 0;
		virtual float GetSize() const = 0;

		/*
		Sets the strata (the rendering order) of the Image. By default 500 (0.5f, the middle). Higher strata = further away = will be behind other images that are closer.
		Internally 0.0f - 1.0f resolution is used, but when setting and getting the value I devide / multiply by 1000, to make the range 0.0f - 1000.0f for outside usage.
		Engine uses 10.0f for splash screens / loading screens.
		*/
		virtual void SetStrata(float strata) = 0;
		virtual float GetStrata() const = 0;
	};
}