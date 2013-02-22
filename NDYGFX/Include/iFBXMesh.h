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

	public:
		// Set animation by index
		virtual bool SetAnimation(unsigned int ani) = 0;

		// Set animation by name
		virtual bool SetAnimation(const char* name) = 0;

		// Binds Mesh To Follow Bone
		// Returns false if bone wasn't found
		// Note: Do Not Delete Mesh While It's Bound, Will Cause Crash!
		virtual bool BindMesh(const char* boneName, iMesh* mesh) = 0;

		// Unbind Mesh From Skeleton
		virtual void UnbindMesh(iMesh* mesh) = 0;
	};
}