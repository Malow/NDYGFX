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
	// Properties
	std::string m_strName;
	int	m_ModelIndex;
	bool m_IsClone;

	// Transformation
	BTHFBX_MATRIX	m_matGeometricOffset2;
	BTHFBX_MATRIX	m_matAnimationTransform2;

	// Animation
	Dictionary<AnimationKeyFrames*> m_AnimationKeyFrames;

	// Model Parts
	std::vector<ModelPart*> m_ModelParts;

public:
	Model(const std::string& strName, int modelIndex, bool isClone);
	Model(Model* srcModel);
	virtual ~Model();

	// Model Parts
	virtual unsigned int GetModelPartCount() { return m_ModelParts.size(); }
	virtual IBTHFbxModelPart* GetModelPart(unsigned int index) { return m_ModelParts[index]; }
	inline std::vector<ModelPart*>& GetModelParts() { return m_ModelParts; }

	// Logic
	void AddVertex(Material* pMaterial, const BTHFBX_VEC3& vPosition, const BTHFBX_VEC3& vNormal, const BTHFBX_VEC3& vTangent, const BTHFBX_VEC2& vTexCoord, const BoneWeights& boneWeights);
	void ProcessSkeleteonBoundingBoxes(Skeleton* skeleton);
	void SetGeometricOffset2(const FbxMatrix& matGeometricOffset);	
	void SetAnimationTransform2(const FbxMatrix& matAnimationTransform);
	void ProcessBlendWeights();

	// Animation
	void AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames);
	AnimationKeyFrames* GetAnimationKeyFrames(const std::string& strAnimationName);
	void UpdateAnimation(AnimationController* pAnimationController);
	inline unsigned int GetAnimationCount() { return m_AnimationKeyFrames.GetCount(); }
	inline AnimationKeyFrames* GetAnimationKeyFrames( unsigned int index ) { return m_AnimationKeyFrames.GetValueAt(index); }

	// Inherited Data Access
	virtual float* GetGeometricOffset()	{ return (float*)m_matGeometricOffset2.f; }
	virtual float* GetAnimationTransform() { return (float*)m_matAnimationTransform2.f; }
	virtual const char* GetName() { return m_strName.c_str(); }
};
