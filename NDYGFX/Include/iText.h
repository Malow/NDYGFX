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