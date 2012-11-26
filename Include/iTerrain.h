#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"


extern "C"
{
	class DECLDIR iTerrain : public virtual iMesh
	{	
	public:
		iTerrain() {}
		virtual ~iTerrain() {}

		virtual bool LoadAndApplyHeightMap(const char* fileName) = 0;
		virtual void filter(unsigned int smootheness) = 0;
		virtual void calculateNormals() = 0;
		virtual float getYPositionAt(float x, float z) = 0;
	};
}