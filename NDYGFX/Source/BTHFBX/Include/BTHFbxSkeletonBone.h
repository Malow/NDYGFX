#ifndef __bthfbx_skeleton_bone_h__
#define __bthfbx_skeleton_bone_h__

#include "BTHFbxCommon.h"

extern "C"
{
	class DECLDIR IBTHFbxSkeletonBone
	{
	public:
		virtual void Update(const float* matNewCombinedTransform) = 0;


		virtual const float* GetBindPoseTransform() = 0;
		virtual const float* GetInvBindPoseTransform() = 0;
		virtual const float* GetBoneReferenceTransform() = 0;
		virtual const float* GetInvBoneReferenceTransform() = 0;
		virtual const float* GetLocalTransform() = 0;
		virtual const float* GetCombinedTransform() = 0;

		virtual void SetLocalTransform(const float* matLocalTransform) = 0;

		virtual void SetTransform(const float* matTransform) = 0;
		virtual const char* GetName() = 0;

		virtual int GetBoneIndex() = 0;
		
		virtual BTHFBX_OBB_DATA GetBoundingBox() = 0;

		virtual void SetAccumulateParentTransformation(bool doAccumulate) = 0;
		virtual bool GetAccumulateParentTransformation() = 0;
	};
}

#endif