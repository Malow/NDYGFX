#include "ModelPart.h"
#include "Model.h"


//--------------------------------------------------------------------------------------
ModelPart::ModelPart(Model* pModelParent, Material* pMaterial) :
	m_pModelParent(pModelParent),
	m_pMaterial(pMaterial),
	m_bSkinnedModel(false)
{
}

//--------------------------------------------------------------------------------------
ModelPart::~ModelPart()
{
	m_pMaterial = NULL;
}

//--------------------------------------------------------------------------------------
void ModelPart::AddVertex(const BTHFBX_VEC3& vPosition, const BTHFBX_VEC3& vNormal, const BTHFBX_VEC3& vTangent, const BTHFBX_VEC2& vTexCoord, const BoneWeights& boneWeights)
{
	m_Positions.push_back(vPosition);
	m_Normals.push_back(vNormal);
	m_Tangents.push_back(vTangent);
	m_TexCoords.push_back(vTexCoord);
	m_BoneWeights.push_back(boneWeights);

	m_VertexIndices.push_back(m_VertexIndices.size());

	if( boneWeights.GetBoneWeightCount() > 0 )
		m_bSkinnedModel = true;

}

void ModelPart::ProcessBlendWeights()
{
	BTTBLENDWEIGHTS BlendWeights;
	BTHFBX_BLEND_WEIGHT_DATA blendData;
	if( m_BoneWeights.size() > 0 )
	{	
		for( int i = 0; i < (int)m_BoneWeights.size(); ++i )
		{
			BlendWeights = m_BoneWeights.at(i).GetBlendWeights();

			blendData.BoneIndices[0] = BlendWeights.BlendIndices.x;
			blendData.BoneIndices[1] = BlendWeights.BlendIndices.y;
			blendData.BoneIndices[2] = BlendWeights.BlendIndices.z;
			blendData.BoneIndices[3] = BlendWeights.BlendIndices.w;

			blendData.BlendWeights[0] = BlendWeights.BlendWeights.x;
			blendData.BlendWeights[1] = BlendWeights.BlendWeights.y;
			blendData.BlendWeights[2] = BlendWeights.BlendWeights.z;
			blendData.BlendWeights[3] = BlendWeights.BlendWeights.w;

			//blendData.NumWeights = m_BoneWeights.at(i).GetBoneWeightCount();
			//for(int j = 0; j < blendData.NumWeights; j++)
			/*
			for(int j = 0; j < m_BoneWeights.at(i).GetBoneWeightCount(); j++)
			{
				blendData.BoneIndices[j] = (unsigned int)m_BoneWeights.at(i).GetBlendWeights().BlendIndices[j];
				blendData.BlendWeights[j] = m_BoneWeights.at(i).GetBlendWeights().BlendWeights[j];
			}
			*/

			m_BlendWeights.push_back(blendData);
		}
	}
}

void ModelPart::ProcessSkeleteonBoundingBoxes(Skeleton* skeleton)
{
	SkeletonBone* bone = NULL;
	BTHFBX_BLEND_WEIGHT_DATA* bw = NULL;
	for(int i = 0; i < (int)m_BlendWeights.size(); i++)
	{
		bw = &m_BlendWeights[i];
		for(int j = 0; j < 4; j++)
		{
			if(bw->BlendWeights[j] > 0.0f)
			{
				bone = skeleton->GetSkeletonBone((int)bw->BoneIndices[j]);

				if(bone)
					bone->UpdateBoundingBoxDataFromVertex(m_Positions[i]);
			}
		}
	}
}