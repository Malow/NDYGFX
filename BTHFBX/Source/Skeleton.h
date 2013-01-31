#pragma once

#include "BoneWeights.h"
#include "SkeletonBone.h"
#include "AnimationController.h"
#include "Dictionary.h"

#include <BTHFbxSkeleton.h>
#include "FBXSDK.h"

class Effect;

class Skeleton : public IBTHFbxSkeleton
{
public:
	Skeleton();
	~Skeleton();

	void AddSkeletonBone(SkeletonBone* pSkeletonBone);
	SkeletonBone* FindBone(const std::string& strBoneName);

	virtual IBTHFbxSkeletonBone* GetBone(const char* boneName);
	virtual IBTHFbxSkeletonBone* GetBone(int index);

	int FindBoneIndex(const std::string& strBoneName);

	SkeletonBone* GetSkeletonBone(int nIndex);
	const Dictionary<SkeletonBone*>& GetSkeletonBones() { return m_SkeletonBones; }

	//void SetSkinTransform(int index, D3DXMATRIX& matSkinTransform);
	//D3DXMATRIX* GetSkinTransforms() { return m_SkinTransforms; }

	void SetSkinTransform2(int index, FbxMatrix& matSkinTransform);
	virtual float* GetSkinTransforms() { return (float*)m_SkinTransforms2; }

	int GetBoneCount()				{ return m_nBoneCount; }

	void UpdateAnimation(AnimationController* pAnimationController);
	void UpdateCombinedAABB();
	virtual BTHFBX_AABB_DATA GetCombinedBoundingBox() { return m_CombinedAABB; }
	void BuildBoneHierarchy();

	BTHFBX_MATRIX*	m_SkinTransforms2;
protected:
	Dictionary<SkeletonBone*> m_SkeletonBones;
	//D3DXMATRIX* m_SkinTransforms;
		
	BTHFBX_AABB_DATA m_CombinedAABB;
	SkeletonBone* m_RootBone;

	int m_nBoneCount;
};
