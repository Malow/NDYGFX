#pragma once

#pragma warning(disable : 4995)

#include <map>
#include <string>

#include "AnimationKeyFrames.h"
#include "Dictionary.h"

#include "FBXSDK.h"

#include <BTHFbxSkeletonBone.h>

class AnimationController;
class Skeleton;

class SkeletonBone : public IBTHFbxSkeletonBone
{
public:
	SkeletonBone(std::string strName, int nParentBoneIndex, Skeleton* skeleton);
	~SkeletonBone();

	void AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames);
	void AddChild(SkeletonBone* child);
	//void SetBindPoseTransform(const D3DXMATRIX& matBindPoseTransform);
	//void SetBoneReferenceTransform(const D3DXMATRIX& matBoneReferenceTransform);
	//void SetLocalTransform(const D3DXMATRIX& matLocalTransform);

	void SetBindPoseTransform2(const FbxMatrix& matBindPoseTransform);
	void SetBoneReferenceTransform2(const FbxMatrix& matBoneReferenceTransform);
	void SetLocalTransform2(const FbxMatrix& matLocalTransform);

	void SetLocalTransform(const float* matLocalTransform);

	AnimationKeyFrames* GetAnimationKeyFrames(const std::string strAnimationName);
		
	/*
	const D3DXMATRIX& GetBindPoseTransform()			{ return m_matBindPoseTransform; }
	const D3DXMATRIX& GetInvBindPoseTransform()			{ return m_matInvBindPoseTransform; }
	const D3DXMATRIX& GetBoneReferenceTransform()		{ return m_matBoneReferenceTransform; }
	const D3DXMATRIX& GetInvBoneReferenceTransform()	{ return m_matInvBoneReferenceTransform; }	
	const D3DXMATRIX& GetLocalTransform()				{ return m_matLocalTransform; }
	const D3DXMATRIX& GetCombinedTransform()			{ return m_matCombinedTransform; }
	*/



	const FbxMatrix& GetBindPoseTransform2()			{ return m_matBindPoseTransform2; }
	const FbxMatrix& GetInvBindPoseTransform2()		{ return m_matInvBindPoseTransform2; }
	const FbxMatrix& GetBoneReferenceTransform2()		{ return m_matBoneReferenceTransform2; }
	const FbxMatrix& GetInvBoneReferenceTransform2()	{ return m_matInvBoneReferenceTransform2; }	
	const FbxMatrix& GetLocalTransform2()				{ return m_matLocalTransform2; }
	const FbxMatrix& GetCombinedTransform2()			{ return m_matCombinedTransform2; }

	const float* GetBindPoseTransform();
	const float* GetInvBindPoseTransform();
	const float* GetBoneReferenceTransform();
	const float* GetInvBoneReferenceTransform();
	const float* GetLocalTransform();
	const float* GetCombinedTransform();

	std::string GetNameStr()							{ return m_strName; }
	int GetParentBoneIndex()							{ return m_nParentBoneIndex; }
	
	void SetBoneIndex(int index)						{ m_nBoneIndex = index; }
	int GetBoneIndex()									{ return m_nBoneIndex; }

	void Update(SkeletonBone* parent, AnimationController* pAnimationController, const std::string& strAnimationName);

	void Update(SkeletonBone* parent, const float* matNewCombinedTransform);
	void Update(const float* matNewCombinedTransform);

	virtual void SetTransform(const float* matTransform);
	virtual const char* GetName() { return m_strName.c_str(); }

	void SetAccumulateParentTransformation(bool doAccumulate);
	bool GetAccumulateParentTransformation();

	virtual BTHFBX_OBB_DATA GetBoundingBox() { return m_OBB_Transformed; }

	void UpdateBoundingBoxDataFromVertex(BTHFBX_VEC3 vertexPosition);
	void SetBoundingBoxData(BTHFBX_AABB_DATA aabb);
	void UpdateSceneAABB(BTHFBX_AABB_DATA& aabb);

	FbxMatrix GetSkinTransform() { return m_matSkinTransform; }
	BTHFBX_AABB_DATA GetAABB() { return m_AABB; }

protected:

	Skeleton*	mSkeleton;

	std::string m_strName;

	/*
	D3DXMATRIX m_matBindPoseTransform;
	D3DXMATRIX m_matInvBindPoseTransform;
	D3DXMATRIX m_matBoneReferenceTransform;
	D3DXMATRIX m_matInvBoneReferenceTransform;
	D3DXMATRIX m_matCombinedTransform;
	D3DXMATRIX m_matLocalTransform;
	*/

	BTHFBX_MATRIX m_matBindPoseTransformFloat;
	BTHFBX_MATRIX m_matInvBindPoseTransformFloat;
	BTHFBX_MATRIX m_matBoneReferenceTransformFloat;
	BTHFBX_MATRIX m_matInvBoneReferenceTransformFloat;
	BTHFBX_MATRIX m_matCombinedTransformFloat;
	BTHFBX_MATRIX m_matLocalTransformFloat;

	FbxMatrix m_matBindPoseTransform2;
	FbxMatrix m_matInvBindPoseTransform2;
	FbxMatrix m_matBoneReferenceTransform2;
	FbxMatrix m_matInvBoneReferenceTransform2;
	FbxMatrix m_matCombinedTransform2;
	FbxMatrix m_matLocalTransform2;
	FbxMatrix m_matSkinTransform;

	BTHFBX_AABB_DATA	m_AABB;
	BTHFBX_OBB_DATA		m_OBB;
	BTHFBX_OBB_DATA		m_OBB_Transformed;

	std::vector<SkeletonBone*>	m_Children;

	int m_nBoneIndex;
	int m_nParentBoneIndex;

	bool m_bAccumulateParentTransformation;

	friend class FBXSceneInstance;
	//Dictionary<AnimationKeyFrames*> m_AnimationKeyFrames;
	std::map<std::string, AnimationKeyFrames*> m_AnimationKeyFrames;
};
