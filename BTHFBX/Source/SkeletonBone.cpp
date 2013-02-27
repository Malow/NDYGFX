#include "SkeletonBone.h"
#include "Skeleton.h"
#include "AnimationController.h"
#include "MinMax.h"
#include "FBXCommon.h"


SkeletonBone::SkeletonBone(const std::string& strName, int nParentBoneIndex, Skeleton* skeleton) :
	m_strName(strName),
	m_nParentBoneIndex(nParentBoneIndex),
	mSkeleton(skeleton)
{
	SetAccumulateParentTransformation(true);
	m_AABB.Min = BTHFBX_VEC3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_AABB.Max = BTHFBX_VEC3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

SkeletonBone::~SkeletonBone()
{
	for(auto i = m_AnimationKeyFrames.begin(); i != m_AnimationKeyFrames.end(); i++)
	{
		if ( i->second ) delete i->second, i->second = 0;
	}
}

void SkeletonBone::AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames)
{
	m_AnimationKeyFrames[pAnimationKeyFrames->GetAnimationName()] = pAnimationKeyFrames;
}

void SkeletonBone::AddChild(SkeletonBone* child)
{
	m_Children.push_back(child);
}

void SkeletonBone::Update(SkeletonBone* parent, AnimationController* pAnimationController, const std::string& strAnimationName)
{
	int nKeyFrame = pAnimationController->GetCurrentKeyFrame();
	int nNextKeyFrame = pAnimationController->GetNextKeyFrame();
	float fKeyFrameFactor = pAnimationController->GetKeyFrameFactor();

	bool bUseQuaternionKeyFrames = pAnimationController->UseQuaternionKeyFrames();

	AnimationKeyFrames* pAnimationKeyFrames = GetAnimationKeyFrames(strAnimationName);
	if ( !pAnimationKeyFrames ) return;

	if( bUseQuaternionKeyFrames )
	{
		const AnimationQuaternionKeyFrame& qKeyFrame1 = pAnimationKeyFrames->GetKeyFrameQuaternion(nKeyFrame);
		const AnimationQuaternionKeyFrame& qKeyFrame2 = pAnimationKeyFrames->GetKeyFrameQuaternion(nNextKeyFrame);
		const FbxQuaternion& q1 = qKeyFrame1.GetQuaternion2();
		const FbxQuaternion& q2 = qKeyFrame2.GetQuaternion2();
			
		FbxQuaternion qq;
				
		qq = qSlerp(q1, q2, fKeyFrameFactor);

		const FbxVector4 tt1 = qKeyFrame1.GetTranslation2();
		const FbxVector4 tt2 = qKeyFrame2.GetTranslation2();

		FbxVector4 vTranslation2;

		// lerp
		vTranslation2 = tt1 + (tt2 - tt1) * fKeyFrameFactor;

		FbxVector4 s(1,1,1);

		m_matLocalTransform2.SetTQS(vTranslation2, qq, s);
	}
	else
	{
		m_matLocalTransform2 = pAnimationKeyFrames->GetKeyFrameTransform2(nKeyFrame);
	}

	m_matCombinedTransform2 = m_matLocalTransform2;
	if (parent) m_matCombinedTransform2 = parent->GetCombinedTransform2() * m_matLocalTransform2;

	m_matSkinTransform = m_matCombinedTransform2 * GetInvBindPoseTransform2() * GetBoneReferenceTransform2();
	mSkeleton->SetSkinTransform2(GetBoneIndex(), m_matSkinTransform);

	for(unsigned int i = 0; i < 8; ++i)
	{
		m_OBB_Transformed.Corners[i].x = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 0) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 0) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 0) + (float)m_matSkinTransform.Get(3, 0);
		m_OBB_Transformed.Corners[i].y = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 1) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 1) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 1) + (float)m_matSkinTransform.Get(3, 1);
		m_OBB_Transformed.Corners[i].z = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 2) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 2) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 2) + (float)m_matSkinTransform.Get(3, 2);
	}

	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Update(this, pAnimationController, strAnimationName);
	}
}

void SkeletonBone::Update(SkeletonBone* parent, const float* matNewCombinedTransform)
{
	m_matCombinedTransform2 = m_matLocalTransform2;
	if(parent)
	{
		if(GetAccumulateParentTransformation())
			m_matCombinedTransform2 = parent->GetCombinedTransform2() * m_matLocalTransform2;
		else
			m_matCombinedTransform2 = m_matLocalTransform2;
	}
	else if(matNewCombinedTransform)
	{
		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				m_matCombinedTransform2.Set(y, x, matNewCombinedTransform[y*4+x]);
	}

	m_matSkinTransform = m_matCombinedTransform2 * GetInvBindPoseTransform2() * GetBoneReferenceTransform2();
	mSkeleton->SetSkinTransform2(GetBoneIndex(), m_matSkinTransform);

	for(int i = 0; i < 8; i++)
	{
		//matSkinTransform.SetIdentity();
		m_OBB_Transformed.Corners[i].x = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 0) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 0) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 0) + (float)m_matSkinTransform.Get(3, 0);
		m_OBB_Transformed.Corners[i].y = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 1) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 1) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 1) + (float)m_matSkinTransform.Get(3, 1);
		m_OBB_Transformed.Corners[i].z = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 2) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 2) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 2) + (float)m_matSkinTransform.Get(3, 2);
	}

	for(size_t i = 0; i < m_Children.size(); i++)
		m_Children[i]->Update(this, NULL);
}

void SkeletonBone::Update(const float* matNewCombinedTransform)
{
	SkeletonBone* parent = mSkeleton->GetSkeletonBone(m_nParentBoneIndex);
	Update(parent, matNewCombinedTransform);

	mSkeleton->UpdateCombinedAABB();
}

void SkeletonBone::SetTransform(const float* matTransform)
{
	for(int y = 0; y < 4; y++)
		for(int x = 0; x < 4; x++)
			m_matCombinedTransform2.Set(y, x, matTransform[y*4+x]);

	m_matSkinTransform = m_matCombinedTransform2 * GetInvBindPoseTransform2() * GetBoneReferenceTransform2();
	mSkeleton->SetSkinTransform2(GetBoneIndex(), m_matSkinTransform);

	for(int i = 0; i < 8; i++)
	{
		m_OBB_Transformed.Corners[i].x = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 0) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 0) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 0) + (float)m_matSkinTransform.Get(3, 0);
		m_OBB_Transformed.Corners[i].y = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 1) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 1) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 1) + (float)m_matSkinTransform.Get(3, 1);
		m_OBB_Transformed.Corners[i].z = m_OBB.Corners[i].x * (float)m_matSkinTransform.Get(0, 2) + m_OBB.Corners[i].y * (float)m_matSkinTransform.Get(1, 2) + m_OBB.Corners[i].z * (float)m_matSkinTransform.Get(2, 2) + (float)m_matSkinTransform.Get(3, 2);
	}
}

void SkeletonBone::SetAccumulateParentTransformation(bool doAccumulate)
{
	m_bAccumulateParentTransformation = doAccumulate;
}

bool SkeletonBone::GetAccumulateParentTransformation()
{
	return m_bAccumulateParentTransformation;
}

void SkeletonBone::SetBindPoseTransform2(const FbxMatrix& matBindPoseTransform)
{
	m_matBindPoseTransform2 = matBindPoseTransform;
	m_matInvBindPoseTransform2 = m_matBindPoseTransform2.Inverse();
}

//--------------------------------------------------------------------------------------
void SkeletonBone::SetBoneReferenceTransform2(const FbxMatrix& matBoneReferenceTransform)
{
	m_matBoneReferenceTransform2 = matBoneReferenceTransform;
	m_matInvBoneReferenceTransform2 = m_matBoneReferenceTransform2.Inverse();
}

void SkeletonBone::SetLocalTransform2(const FbxMatrix& matLocalTransform)
{
	m_matLocalTransform2 = matLocalTransform;
}

//--------------------------------------------------------------------------------------
AnimationKeyFrames* SkeletonBone::GetAnimationKeyFrames(const std::string strAnimationName)
{
	std::map<std::string, AnimationKeyFrames*>::iterator i  = m_AnimationKeyFrames.find(strAnimationName);

	if(i != m_AnimationKeyFrames.end() )
	{
		return i->second;
	}
	return NULL;
}

const float* SkeletonBone::GetBindPoseTransform()
{
	FbxMatrixToBTHFBX_MATRIX(m_matBindPoseTransform2, m_matBindPoseTransformFloat);	
	return m_matBindPoseTransformFloat.f;
}

const float* SkeletonBone::GetInvBindPoseTransform()
{
	FbxMatrixToBTHFBX_MATRIX(m_matInvBindPoseTransform2, m_matInvBindPoseTransformFloat);	
	return m_matInvBindPoseTransformFloat.f;
}

const float* SkeletonBone::GetBoneReferenceTransform()
{
	FbxMatrixToBTHFBX_MATRIX(m_matBoneReferenceTransform2, m_matBoneReferenceTransformFloat);	
	return m_matBoneReferenceTransformFloat.f;
}

const float* SkeletonBone::GetInvBoneReferenceTransform()
{
	FbxMatrixToBTHFBX_MATRIX(m_matInvBoneReferenceTransform2, m_matInvBoneReferenceTransformFloat);	
	return m_matInvBoneReferenceTransformFloat.f;
}
	
const float* SkeletonBone::GetLocalTransform()
{
	FbxMatrixToBTHFBX_MATRIX(m_matLocalTransform2, m_matLocalTransformFloat);		
	return m_matLocalTransformFloat.f;
}

const float* SkeletonBone::GetCombinedTransform()
{
	FbxMatrixToBTHFBX_MATRIX(m_matCombinedTransform2, m_matCombinedTransformFloat);		
	return m_matCombinedTransformFloat.f;
}

void SkeletonBone::SetLocalTransform(const float* matLocalTransform)
{
	BTHFBX_MATRIXToFbxMatrix(*(BTHFBX_MATRIX*)matLocalTransform, m_matLocalTransform2);
}

void SkeletonBone::SetBoundingBoxData(BTHFBX_AABB_DATA aabb)
{
	m_AABB = aabb;

	BTHFBX_VEC3& min = m_AABB.Min;
	BTHFBX_VEC3& max = m_AABB.Max;
	
	m_OBB.Corners[0] = BTHFBX_VEC3( min.x, min.y, min.z ); // xyz (min)
	m_OBB.Corners[1] = BTHFBX_VEC3( max.x, min.y, min.z ); // Xyz
	m_OBB.Corners[2] = BTHFBX_VEC3( min.x, max.y, min.z ); // xYz
	m_OBB.Corners[3] = BTHFBX_VEC3( max.x, max.y, min.z ); // XYz
	m_OBB.Corners[4] = BTHFBX_VEC3( min.x, min.y, max.z ); // xyZ
	m_OBB.Corners[5] = BTHFBX_VEC3( max.x, min.y, max.z ); // XyZ
	m_OBB.Corners[6] = BTHFBX_VEC3( min.x, max.y, max.z ); // 
	m_OBB.Corners[7] = BTHFBX_VEC3( max.x, max.y, max.z ); // XYZ (max)
}

void SkeletonBone::UpdateBoundingBoxDataFromVertex(BTHFBX_VEC3 vertexPosition)
{
	m_AABB.Min.x = Min(vertexPosition.x, m_AABB.Min.x);
	m_AABB.Min.y = Min(vertexPosition.y, m_AABB.Min.y);
	m_AABB.Min.z = Min(vertexPosition.z, m_AABB.Min.z);

	m_AABB.Max.x = Max(vertexPosition.x, m_AABB.Max.x);
	m_AABB.Max.y = Max(vertexPosition.y, m_AABB.Max.y);
	m_AABB.Max.z = Max(vertexPosition.z, m_AABB.Max.z);
}

void SkeletonBone::UpdateSceneAABB(BTHFBX_AABB_DATA& aabb)
{
	for(int i = 0; i < 8; i++)
	{
		aabb.Min.x = Min(m_OBB_Transformed.Corners[i].x, aabb.Min.x);
		aabb.Min.y = Min(m_OBB_Transformed.Corners[i].y, aabb.Min.y);
		aabb.Min.z = Min(m_OBB_Transformed.Corners[i].z, aabb.Min.z);

		aabb.Max.x = Max(m_OBB_Transformed.Corners[i].x, aabb.Max.x);
		aabb.Max.y = Max(m_OBB_Transformed.Corners[i].y, aabb.Max.y);
		aabb.Max.z = Max(m_OBB_Transformed.Corners[i].z, aabb.Max.z);
	}

	for(size_t i = 0; i < m_Children.size(); i++)
		m_Children[i]->UpdateSceneAABB(aabb);
}