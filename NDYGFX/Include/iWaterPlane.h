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
	protected:
		virtual ~iWaterPlane() {};

	public:
		iWaterPlane() {};
		
		virtual void SetVertexPosition(Vector3 pos, int vertexIndex) = 0;


	};
}