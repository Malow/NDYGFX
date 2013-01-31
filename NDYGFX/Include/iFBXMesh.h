#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"

extern "C"
{
	class DECLDIR iFBXMesh : public virtual iMesh
	{	
	protected:
		iFBXMesh() {};
		virtual ~iFBXMesh() {};
	};
}