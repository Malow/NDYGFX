#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"

extern "C"
{
	class DECLDIR iAnimatedMesh : public virtual iMesh
	{	
		public:
			iAnimatedMesh() {};
			virtual ~iAnimatedMesh() {};

			virtual void SetCurrentTime(float currentTime) = 0;
			virtual void Update(float deltaTime) = 0;

	};
}