#include "Model.h"

//--------------------------------------------------------------------------------------
Model::Model(const std::string& strName, int modelIndex, bool isClone)
{
	m_strName = strName;
	m_ModelIndex = modelIndex;

	m_IsClone = isClone;

	/*
	D3DXMatrixIdentity(&m_matAbsoluteTransform);
	D3DXMatrixIdentity(&m_matGeometricOffset);
	D3DXMatrixIdentity(&m_matAnimationTransform);
	*/
}

Model::Model(Model* srcModel)
{
	*this = *srcModel;
	m_IsClone = true;
}

//--------------------------------------------------------------------------------------
Model::~Model()
{
	if(m_IsClone == false)
	{
		m_AnimationKeyFrames.RemoveAll(true);
		for( size_t i = 0; i < m_ModelParts.size(); ++i )
		{
			if( m_ModelParts[i] )
			{
				delete m_ModelParts[i];
			}
		}
	}
	m_ModelParts.clear();
}

//--------------------------------------------------------------------------------------
void Model::AddVertex(Material* pMaterial, const BTHFBX_VEC3& vPosition, const BTHFBX_VEC3& vNormal, const BTHFBX_VEC3& vTangent, const BTHFBX_VEC2& vTexCoord, const BoneWeights& boneWeights)
{
	bool bNewMaterial = true;

	for( size_t i = 0; i < m_ModelParts.size(); ++i )
	{
		ModelPart* pModelPart = m_ModelParts[i];
		if( pMaterial == pModelPart->GetMaterial() )
		{
			pModelPart->AddVertex(vPosition, vNormal, vTangent, vTexCoord, boneWeights);
			bNewMaterial = false;
		}
	}

	if( bNewMaterial )
	{
		ModelPart* pModelPart = new ModelPart(this, m_ModelIndex, pMaterial);
		pModelPart->AddVertex(vPosition, vNormal, vTangent, vTexCoord, boneWeights);
		m_ModelParts.push_back(pModelPart);
	}
}

void Model::ProcessBlendWeights()
{
	for( size_t i = 0; i < m_ModelParts.size(); ++i )
	{
		m_ModelParts[i]->ProcessBlendWeights();
	}
}

void Model::ProcessSkeleteonBoundingBoxes(Skeleton* skeleton)
{
	for( size_t i = 0; i < m_ModelParts.size(); ++i )
	{
		m_ModelParts[i]->ProcessSkeleteonBoundingBoxes(skeleton);
	}
}

//--------------------------------------------------------------------------------------
void Model::Optimize()
{
	for( size_t i = 0; i < m_ModelParts.size(); ++i )
	{
		m_ModelParts[i]->Optimize();
	}
}

//--------------------------------------------------------------------------------------
void Model::AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames)
{
	m_AnimationKeyFrames.Add( pAnimationKeyFrames->GetAnimationName(), pAnimationKeyFrames );
}

//--------------------------------------------------------------------------------------
AnimationKeyFrames* Model::GetAnimationKeyFrames(const std::string& strAnimationName)
{
	return m_AnimationKeyFrames.Find(strAnimationName).second;
}

//--------------------------------------------------------------------------------------
void Model::UpdateAnimation(AnimationController* pAnimationController)
{
	if( pAnimationController )
	{
		Animation* pAnimation = pAnimationController->GetCurrentAnimation();

		if( pAnimation )
		{
			AnimationKeyFrames* pAnimationKeyFrames = m_AnimationKeyFrames.Find(pAnimation->GetName(), NULL);

			if( pAnimationKeyFrames )
			{
				int nKeyFrame = pAnimationController->GetCurrentKeyFrame();
				//m_matAnimationTransform = pAnimationKeyFrames->GetKeyFrameTransform(nKeyFrame);

				SetAnimationTransform2(pAnimationKeyFrames->GetKeyFrameTransform2(nKeyFrame));
				return;
			}
		}
	}
	//D3DXMatrixIdentity(&m_matAnimationTransform);

	FbxMatrix tmp;
	tmp.SetIdentity();
	SetAnimationTransform2(tmp);
}