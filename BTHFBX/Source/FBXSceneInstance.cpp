#include "FBXSceneInstance.h"
#include "FBXScene.h"
#include "AABB.h"
#include "Math.h"



FBXSceneInstance::FBXSceneInstance(class FBXScene* scene) :
	m_pSkeleton(NULL), 
	m_pAnimationController(NULL),
	mScene(scene),
	m_Curves(NULL)
{
}

FBXSceneInstance::~FBXSceneInstance()
{
	//since the keyframes are pointers the source instance should delete them
	if(m_pSkeleton)
	{
		for(unsigned int i = 0; i < m_pSkeleton->GetBoneCount(); i++)
		{
			m_pSkeleton->GetSkeletonBone(i)->m_AnimationKeyFrames.clear();
		}
	}

	m_Models.RemoveAll(true);
	if (m_pSkeleton) delete m_pSkeleton, m_pSkeleton = 0;
	if (m_pAnimationController) delete m_pAnimationController, m_pAnimationController = 0;
}

void FBXSceneInstance::InitInstance()
{
	// Bounding Box
	m_BoundingBox = mScene->m_BoundingBox;

	// Models
	for(unsigned int i = 0; i < mScene->GetModelCount(); i++)
	{
		Model* srcModel = mScene->GetModel(i);
		m_Models.Add(srcModel->GetName(), new Model(srcModel));
	}

	// Copy Skeleton
	if ( mScene->GetSkeleton() )
	{
		Skeleton* newSkeleton = new Skeleton();
		
		for( unsigned int x = 0; x < mScene->GetSkeleton()->GetBoneCount(); ++x )
		{
			SkeletonBone* sourceBone = mScene->GetSkeleton()->GetSkeletonBone(x);

			SkeletonBone* targetBone = new SkeletonBone(
				sourceBone->GetName(), 
				sourceBone->GetParentBoneIndex(), 
				newSkeleton);

			newSkeleton->AddSkeletonBone( targetBone );
		}

		newSkeleton->BuildBoneHierarchy();

		for( unsigned int x = 0; x < newSkeleton->GetBoneCount(); ++x )
		{
			SkeletonBone* sourceBone = mScene->GetSkeleton()->GetSkeletonBone(x);
			SkeletonBone* targetBone = newSkeleton->GetSkeletonBone(x);
			targetBone->SetBindPoseTransform2(sourceBone->GetBindPoseTransform2());
			targetBone->SetBoneReferenceTransform2(sourceBone->GetBoneReferenceTransform2());
			targetBone->m_AnimationKeyFrames = sourceBone->m_AnimationKeyFrames;
			targetBone->SetBoundingBoxData(sourceBone->m_AABB);
		}

		// Update My Skeleton
		m_pSkeleton = newSkeleton;

		// Update My Animation
		m_pAnimationController = new AnimationController(mScene->m_pAnimationController);
	}

	// Curves
	m_Curves = &mScene->m_Curves;
}

BTHFBX_RAY_BOX_RESULT FBXSceneInstance::RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix)
{
	BTHFBX_RAY_BOX_RESULT result;
	result.DistanceToHit = -1.0f;
	result.JointIndex = -1;

	FbxVector4 o(ray.Origin.x, ray.Origin.y, ray.Origin.z);
	FbxVector4 d(ray.Direction.x, ray.Direction.y, ray.Direction.z);
	FbxMatrix world, worldInv;
	
	for(int y = 0; y < 4; y++)
		for(int x = 0; x < 4; x++)
			world.Set(y, x, worldMatrix->f[y*4+x]);

	worldInv = world.Inverse();

	FbxVector4 no, nd;
	FBXVec3TransformCoord(&no, &o, &worldInv);
	FBXVec3TransformNormal(&nd, &d, &worldInv);
	nd.Normalize();

	float hitDistance = -1.0f;
	
	if(!m_pSkeleton)
	{
		// FUNKAR INTE?=!?!??!?!!?
		hitDistance = BoxVsRay(no, nd, m_BoundingBox);
		result.DistanceToHit = hitDistance;
	}
	else
	{
		hitDistance = BoxVsRay(no, nd, m_pSkeleton->GetCombinedBoundingBox());
		result.DistanceToHit = hitDistance;

		if(hitDistance > -1.0f && m_pSkeleton)
		{
			SkeletonBone* bone = NULL;
			int jointIndex = -1;
			float tmpDist, closestDist = FLT_MAX;

			for(unsigned int i = 0; i < m_pSkeleton->GetBoneCount(); i++)
			{
				bone = (SkeletonBone*)m_pSkeleton->GetBone(i);

				worldInv = (world * bone->GetSkinTransform()).Inverse();
				FBXVec3TransformCoord(&no, &o, &worldInv);
				FBXVec3TransformNormal(&nd, &d, &worldInv);
				nd.Normalize();

				tmpDist = BoxVsRay(no, nd, bone->GetAABB());

				if(tmpDist > -1.0f)
				{
					if(tmpDist < closestDist)
					{
						closestDist = tmpDist;
						jointIndex = i;
					}
				}
			}

			if(jointIndex != -1)
			{
				result.DistanceToHit = closestDist;
				result.JointIndex = jointIndex;
			}
		}
	}

	return result;
}

unsigned int FBXSceneInstance::GetModelCount()
{
	return m_Models.GetCount();
}

IBTHFbxModel* FBXSceneInstance::GetModel(unsigned int index)
{
	return m_Models.GetValueAt(index);
}

unsigned int FBXSceneInstance::GetCurveCount()
{
	return m_Curves->size();
}

IBTHFbxCurve* FBXSceneInstance::GetCurve(unsigned int index)
{
	return &(*m_Curves)[index];
}

bool FBXSceneInstance::IsAnimated()
{
	if( m_pAnimationController ) { return m_pAnimationController->GetAnimationCount() > 0; } return false; 
}

bool FBXSceneInstance::IsSkinned() 
{
	return m_pSkeleton != NULL;
}

IBTHFbxAnimationController* FBXSceneInstance::GetAnimationController()
{
	return m_pAnimationController;
}
	
IBTHFbxSkeleton* FBXSceneInstance::GetSkeleton()
{
	return m_pSkeleton;
}

BTHFBX_AABB_DATA FBXSceneInstance::GetBoundingBoxData()
{
	return m_BoundingBox;
}

void FBXSceneInstance::UpdateScene(float fElapsedTime, bool bEnableAnimation)
{
	if( m_pAnimationController && bEnableAnimation )
	{
		m_pAnimationController->Update(fElapsedTime);

		if ( m_pSkeleton )
		{
			m_pSkeleton->UpdateAnimation(m_pAnimationController);
		}

		for( unsigned int i = 0; i < m_Models.GetCount(); ++i )
		{
			m_Models.GetValueAt(i)->UpdateAnimation(m_pAnimationController);
		}
	}
}

void FBXSceneInstance::ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attributeType)
{
	if( !pNode )
		return;

	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute)
	{
		if( pNodeAttribute->GetAttributeType() == attributeType )
		{
			switch(pNodeAttribute->GetAttributeType())
			{
			case FbxNodeAttribute::EType::eSkeleton:
				{
					ProcessSkeleton(pNode);
					break;
				}

			default:
				break;
			};
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
	{
		ProcessNode(pNode->GetChild(i), attributeType);
	}
}

void FBXSceneInstance::ProcessSkeleton(FbxNode* pNode)
{
	FbxSkeleton* pFBXSkeleton = pNode->GetSkeleton();
	if( !pFBXSkeleton )
		return;
	
	if( !m_pSkeleton )
	{
		m_pSkeleton = new Skeleton();
	}

	int nParentBoneIndex = -1;
	FbxNode* pParentNode = pNode->GetParent();

	if( pParentNode )
		nParentBoneIndex = m_pSkeleton->FindBoneIndex(pParentNode->GetName());

	SkeletonBone* pSkeletonBone = new SkeletonBone( pNode->GetName(), nParentBoneIndex, m_pSkeleton );
	m_pSkeleton->AddSkeletonBone(pSkeletonBone);
	
	int boneIndex = m_pSkeleton->FindBoneIndex(pNode->GetName());
	pSkeletonBone->SetBoneIndex(boneIndex);
}

bool FBXSceneInstance::HasFBXAnimation(FbxNode *pNode) 
{ 
	FbxTimeSpan timeSpan;
	return pNode->GetAnimationInterval(timeSpan);
} 

FbxMatrix FBXSceneInstance::GetAbsoluteTransformFromCurrentTake2(FbxNode* pNode, FbxTime time)
{
	if( !pNode )
	{
		FbxMatrix mat;
		mat.SetIdentity();
		return mat;
	}

	FbxMatrix matFBXGlobal = mScene->GetScene()->GetEvaluator()->GetNodeGlobalTransform(pNode, time);
	return matFBXGlobal;
}