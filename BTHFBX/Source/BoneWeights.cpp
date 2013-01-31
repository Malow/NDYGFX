#include "BoneWeights.h"

//--------------------------------------------------------------------------------------
BoneWeights::BoneWeights() : m_fWeightSum(0)
{
}

//--------------------------------------------------------------------------------------
void BoneWeights::AddBoneWeight(int nBoneIndex, float fBoneWeight)
{
	if( fBoneWeight <= 0.0f )
		return;

	m_fWeightSum += fBoneWeight;

	float bAdded = false;
	for(std::vector<std::pair<int,float>>::iterator it = m_boneWeights.begin(); it != m_boneWeights.end(); ++it )
	{
		if( fBoneWeight > it->second )
		{
			m_boneWeights.insert(it, std::make_pair(nBoneIndex, fBoneWeight));
			bAdded = true;
			break;
		}
	}
	if( !bAdded )
		m_boneWeights.push_back(std::make_pair(nBoneIndex,fBoneWeight));
}

//--------------------------------------------------------------------------------------
void BoneWeights::Validate()
{
	m_fWeightSum = 0.0f;
	int nIndex = 0;

	std::vector<std::pair<int,float>>::iterator it = m_boneWeights.begin();
	while(it != m_boneWeights.end())
	{
		if( nIndex >= MAXBONES_PER_VERTEX )
		{
			it = m_boneWeights.erase(it);
		}
		else
		{
			m_fWeightSum += it->second;
			++nIndex;
			++it;
		}
	}
}

//--------------------------------------------------------------------------------------
void BoneWeights::Normalize()
{
	Validate();

	float fScale = 1.0f/m_fWeightSum;
	std::vector<std::pair<int,float>>::iterator it = m_boneWeights.begin();
	while(it != m_boneWeights.end())
	{
		it->second *= fScale;
		++it;
	}
}

//--------------------------------------------------------------------------------------
BTTBLENDWEIGHTS BoneWeights::GetBlendWeights()
{
	BTTBLENDWEIGHTS blendWeights;

	int nIndex = 0;
	std::vector<std::pair<int,float>>::iterator it = m_boneWeights.begin();
	while( it != m_boneWeights.end() )
	{
		blendWeights.SetBlendWeight(nIndex, it->first, it->second);
		++nIndex;
		++it;

		if( nIndex >= MAXBONES_PER_VERTEX )
			break;
	}

	return blendWeights;
}