#pragma once

#include "BoneWeights.h"
#include "SkeletonBone.h"
#include "AnimationController.h"
#include "Dictionary.h"

#include <BTHFbxSkeleton.h>
#include "FBXSDK.h"


class Skeleton : public IBTHFbxSkeleton
{
protected:
	Dictionary<SkeletonBone*>	m_SkeletonBones;		
	BTHFBX_AABB_DATA			m_CombinedAABB;
	SkeletonBone*				m_RootBone;
	BTHFBX_MATRIX*				m_SkinTransforms2;
	unsigned int				m_nBoneCount;

public:
	Skeleton();
	virtual ~Skeleton();

	// Procedural Data Access
	SkeletonBone* FindBone(const std::string& strBoneName);
	int FindBoneIndex(const std::string& strBoneName);
	SkeletonBone* GetSkeletonBone(int nIndex);

	// Direct Data Access
	inline const Dictionary<SkeletonBone*>& GetSkeletonBones() { return m_SkeletonBones; }
	inline unsigned int GetBoneCount() const { return m_SkeletonBones.GetCount(); }
	inline virtual float* GetSkinTransforms() { return (float*)m_SkinTransforms2; }

	// Logic
	void SetSkinTransform2(int index, FbxMatrix& matSkinTransform);
	void UpdateAnimation(AnimationController* pAnimationController);
	void UpdateCombinedAABB();
	void BuildBoneHierarchy();
	void AddSkeletonBone(SkeletonBone* pSkeletonBone);

	// Overriden Functions
	virtual BTHFBX_AABB_DATA GetCombinedBoundingBox() { return m_CombinedAABB; }
	virtual IBTHFbxSkeletonBone* GetBone(const char* boneName);
	virtual IBTHFbxSkeletonBone* GetBone(int index);
};
