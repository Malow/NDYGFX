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
	};
}