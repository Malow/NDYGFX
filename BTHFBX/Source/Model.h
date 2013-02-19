#pragma once

#include "ModelPart.h"
#include "Material.h"
#include "BoneWeights.h"
#include "AnimationController.h"
#include "AnimationKeyFrames.h"

#include "../Include/BTHFbxModel.h"

class Model : public IBTHFbxModel
{
protected:
	std::string m_strName;
	int	m_ModelIndex;
	bool m_IsClone;

	BTHFBX_MATRIX m_matGeometricOffset2;
	BTHFBX_MATRIX m_matAnimationTransform2;
	float	m_matAbsoluteTransform2[16];

	Dictionary<AnimationKeyFrames*> m_AnimationKeyFrames;
	std::vector<ModelPart*> m_ModelParts;

public:
	Model(const std::string& strName, int modelIndex, bool isClone);
	Model(Model* srcModel);
	virtual ~Model();

	// Model Parts
	virtual int GetModelPartCount() { return (int)m_ModelParts.size(); }
	virtual IBTHFbxModelPart* GetModelPart(int index) { return m_ModelParts[index]; }
	inline std::vector<ModelPart*>& GetModelParts() { return m_ModelParts; }

	void AddVertex(Material* pMaterial, const BTHFBX_VEC3& vPosition, const BTHFBX_VEC3& vNormal, const BTHFBX_VEC3& vTangent, const BTHFBX_VEC2& vTexCoord, const BoneWeights& boneWeights);

	void ProcessBlendWeights();

	void AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames);
	AnimationKeyFrames* GetAnimationKeyFrames(const std::string& strAnimationName);

	void UpdateAnimation(AnimationController* pAnimationController);

	//void SetAbsoluteTransform(const D3DXMATRIX& matAbsoluteTransform)   { m_matAbsoluteTransform = matAbsoluteTransform; }
	//void SetGeometricOffset(const D3DXMATRIX& matGeometricOffset)		{ m_matGeometricOffset = matGeometricOffset; }
	//D3DXMATRIX GetAbsoluteTransform() { return m_matAbsoluteTransform; }
	//D3DXMATRIX GetGeometricOffset()	{ return m_matGeometricOffset; }
	//D3DXMATRIX GetAnimationTransform() { return m_matAnimationTransform; }

	virtual float* GetGeometricOffset()	{ return (float*)m_matGeometricOffset2.f; }
	virtual float* GetAnimationTransform() { return (float*)m_matAnimationTransform2.f; }

	void ProcessSkeleteonBoundingBoxes(Skeleton* skeleton);

	void SetAbsoluteTransform2(const FbxMatrix& matAbsoluteTransform)  
	{
		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				m_matAbsoluteTransform2[y*4+x] = (float)matAbsoluteTransform.Get(y, x);
	}
	
	void SetGeometricOffset2(const FbxMatrix& matGeometricOffset)
	{
		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				m_matGeometricOffset2.f[y*4+x] = (float)matGeometricOffset.Get(y, x);
	}
	
	void SetAnimationTransform2(const FbxMatrix& matAnimationTransform)
	{
		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				m_matAnimationTransform2.f[y*4+x] = (float)matAnimationTransform.Get(y, x);
	}
		
	virtual const char* GetName() { return m_strName.c_str(); }
};
