#ifndef __bthfbx_skeleton_h__
#define __bthfbx_skeleton_h__

#include "BTHFbxCommon.h"
#include "BTHFbxSkeletonBone.h"

extern "C"
{
	class DECLDIR IBTHFbxSkeleton
	{
	public:
		virtual unsigned int GetBoneCount() const = 0;

		virtual float* GetSkinTransforms() = 0;

		virtual IBTHFbxSkeletonBone* GetBone(const char* boneName) = 0;
		virtual IBTHFbxSkeletonBone* GetBone(int index) = 0;

		virtual BTHFBX_AABB_DATA GetCombinedBoundingBox() = 0;
	};
}

#endif