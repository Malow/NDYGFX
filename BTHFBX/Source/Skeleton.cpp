#include "Skeleton.h"
#include <algorithm>


Skeleton::Skeleton()
{
	m_nBoneCount = 0;
	m_SkinTransforms2 = NULL;
}

Skeleton::~Skeleton()
{
	m_SkeletonBones.RemoveAll(true);
	if ( m_SkinTransforms2 ) delete [] m_SkinTransforms2, m_SkinTransforms2 = 0;
}

void Skeleton::AddSkeletonBone(SkeletonBone* pSkeletonBone)
{
	std::string name = pSkeletonBone->GetName();
	std::transform(name.begin(), name.end(), name.begin(), toupper);
	pSkeletonBone->SetBoneIndex(m_SkeletonBones.GetCount());
	m_SkeletonBones.Add(name, pSkeletonBone);

	if(pSkeletonBone->GetParentBoneIndex() < 0)
		m_RootBone = pSkeletonBone;

	m_nBoneCount++;
}

SkeletonBone* Skeleton::FindBone(const std::string& strBoneName)
{
	std::string strBoneToFind = strBoneName;
	std::transform(strBoneToFind.begin(), strBoneToFind.end(), strBoneToFind.begin(), toupper); 
	std::pair<int, SkeletonBone*> bonePair = m_SkeletonBones.Find(strBoneToFind);

	if( bonePair.first >= 0 )
		return bonePair.second;
	return NULL;
}

IBTHFbxSkeletonBone* Skeleton::GetBone(const char* boneName)
{
	return dynamic_cast<IBTHFbxSkeletonBone*>(FindBone(std::string(boneName)));
}

IBTHFbxSkeletonBone* Skeleton::GetBone(int index)
{
	return dynamic_cast<IBTHFbxSkeletonBone*>(GetSkeletonBone(index));
}

int Skeleton::FindBoneIndex(const std::string& strBoneName)
{
	std::string strBoneToFind = strBoneName;
	std::transform(strBoneToFind.begin(), strBoneToFind.end(), strBoneToFind.begin(), toupper); 
	std::pair<int, SkeletonBone*> bonePair = m_SkeletonBones.Find(strBoneToFind);
	return bonePair.first;
}

void Skeleton::BuildBoneHierarchy()
{
	const std::vector<std::pair<std::string,SkeletonBone*>>& skeletonBones = m_SkeletonBones.GetDataArray();
	std::vector<std::pair<std::string,SkeletonBone*>>::const_iterator it = skeletonBones.begin();

	for(; it != skeletonBones.end(); ++it )
	{
		int index = it->second->GetParentBoneIndex();
		if(index != -1)
		{
			SkeletonBone* bone = GetSkeletonBone(index);
			if(bone)
			{
				bone->AddChild(it->second);
			}
		}
	}

	if( !m_SkinTransforms2 )
	{
		m_SkinTransforms2 = new BTHFBX_MATRIX[m_nBoneCount];
		memset(m_SkinTransforms2, 0, sizeof(BTHFBX_MATRIX) * m_nBoneCount);
	}
}

SkeletonBone* Skeleton::GetSkeletonBone(int nIndex)
{
	if(nIndex < 0)
		return NULL;

	return m_SkeletonBones.GetDataArray()[nIndex].second;
}

void Skeleton::UpdateCombinedAABB()
{
	m_CombinedAABB.Min = BTHFBX_VEC3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_CombinedAABB.Max = BTHFBX_VEC3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_RootBone->UpdateSceneAABB(m_CombinedAABB);
}

void Skeleton::UpdateAnimation(AnimationController* pAnimationController)
{
	Animation* pCurrentAnimation = pAnimationController->GetCurrentAnimation();

	if( !pCurrentAnimation )
		return;

	m_RootBone->Update(NULL, pAnimationController, pCurrentAnimation->GetName());

	UpdateCombinedAABB();
}

void Skeleton::SetSkinTransform2(int index, FbxMatrix& matSkinTransform)
{
	for(int y = 0; y < 4; y++)
		for(int x = 0; x < 4; x++)
			m_SkinTransforms2[index].f[y*4+x] = (float)matSkinTransform.Get(y, x);
}