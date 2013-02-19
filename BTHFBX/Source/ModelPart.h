#pragma once

#include "Material.h"
#include "BoneWeights.h"

#include "Skeleton.h"

#include "BTHFbxModelPart.h"
#include "BTHFbxMaterial.h"

class Model;
class Effect;

class ModelPart : public IBTHFbxModelPart
{
	std::vector<BTHFBX_VEC3>				m_Positions;
	std::vector<BTHFBX_VEC3>				m_Normals;
	std::vector<BTHFBX_VEC3>				m_Tangents;
	std::vector<BTHFBX_VEC2>				m_TexCoords;
	std::vector<unsigned int>				m_VertexIndices;
	std::vector<BoneWeights>				m_BoneWeights;
	std::vector<BTHFBX_BLEND_WEIGHT_DATA>	m_BlendWeights;
	Material*								m_pMaterial;
	Model*									m_pModelParent;
	bool									m_bSkinnedModel;

public:
	ModelPart(Model* pModelParent, int pModelParentIndex, Material* pMaterial);
	~ModelPart();

	void AddVertex(const BTHFBX_VEC3& vPosition, const BTHFBX_VEC3& vNormal, const BTHFBX_VEC3& vTangent, const BTHFBX_VEC2& vTexCoord, const BoneWeights& boneWeights);

	//virtual Material* GetMaterial() { return m_pMaterial; }
	virtual IBTHFbxMaterial* GetMaterial() { return m_pMaterial; }

	void ProcessBlendWeights();

	virtual int GetVertexCount() { return (int)m_Positions.size(); }
	virtual BTHFBX_VEC3* GetVertexPositionData() { return &m_Positions[0]; }
	virtual BTHFBX_VEC3* GetVertexNormalData() { return &m_Normals[0]; }
	virtual BTHFBX_VEC3* GetVertexTangentData() { return &m_Tangents[0]; }
	virtual BTHFBX_VEC2* GetVertexTexCoordData() { return &m_TexCoords[0]; }
	virtual BTHFBX_BLEND_WEIGHT_DATA* GetVertexBoneWeightData()  { return &m_BlendWeights[0]; }
	virtual int GetIndexCount() { return (int)m_VertexIndices.size(); }
	virtual unsigned int* GetIndexData() { return (unsigned int*)&m_VertexIndices[0]; }

	virtual bool IsSkinnedModel() { return m_bSkinnedModel; }

	void ProcessSkeleteonBoundingBoxes(Skeleton* skeleton);
};