#pragma once

#include <vector>
#include <BTHFbxCommon.h>

#pragma warning(disable : 4995)

#define MAXBONES_PER_VERTEX 4
#define MAXBONES_PER_MESH   100

struct BTTBLENDWEIGHTS
{
	BTHFBX_VEC4 BlendIndices;
	BTHFBX_VEC4 BlendWeights;

	BTTBLENDWEIGHTS()
	{
		BlendIndices = BTHFBX_VEC4(0.0f, 0.0f, 0.0f, 0.0f);
		BlendWeights = BTHFBX_VEC4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void SetBlendWeight(int nIndex, int nBoneIndex, float fWeight)
	{
		switch(nIndex)
		{
		case 0:
			BlendIndices.x = (float)nBoneIndex;
			BlendWeights.x = fWeight;
		break;
		case 1:
			BlendIndices.y = (float)nBoneIndex;
			BlendWeights.y = fWeight;
		break;
		case 2:
			BlendIndices.z = (float)nBoneIndex;
			BlendWeights.z = fWeight;
		break;
		case 3:
			BlendIndices.w = (float)nBoneIndex;
			BlendWeights.w = fWeight;
		break;
		}
	}
};

class BoneWeights
{
public:
	BoneWeights();

	void AddBoneWeight(int nBoneIndex, float fBoneWeight);
	inline void AddBoneWeight(std::pair<int,float> boneWeightPair) { AddBoneWeight(boneWeightPair.first, boneWeightPair.second); }
	inline void AddBoneWeights(const BoneWeights& boneWeights) { for( size_t i = 0; i < boneWeights.GetBoneWeightCount(); ++i)	{ AddBoneWeight(boneWeights.GetBoneWeight((int)i)); } }

	void Validate();
	void Normalize();
		
	inline size_t GetBoneWeightCount() const						 { return m_boneWeights.size(); }
	inline std::pair<int,float> GetBoneWeight(int nIndex) const  { return m_boneWeights.at(nIndex); }
	BTTBLENDWEIGHTS GetBlendWeights();
protected:
	std::vector<std::pair<int,float>> m_boneWeights;	// <BoneIndex,BoneWeight>
	float m_fWeightSum;
};