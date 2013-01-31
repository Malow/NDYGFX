#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"

extern "C"
{
	class DECLDIR iWaterPlane : public virtual iMesh 
	{	
	public:
		iWaterPlane() {};
		virtual ~iWaterPlane() {};

	};
}