#pragma once

#include <vector>
#include <string>
#include "FBXSDK.h"


class AnimationQuaternionKeyFrame
{
	FbxQuaternion m_Quaternion2;
	FbxVector4 m_vTranslation2;

public:

	AnimationQuaternionKeyFrame(const FbxMatrix& matTransform2=FbxMatrix())
	{
		FbxVector4 tmp0, tmp1;
		double tmpD;
		matTransform2.GetElements(m_vTranslation2, m_Quaternion2, tmp0, tmp1, tmpD);
	}

	inline const FbxQuaternion& GetQuaternion2() const	{ return m_Quaternion2; }
	inline const FbxVector4& GetTranslation2() const	{ return m_vTranslation2; }
};

class AnimationKeyFrames
{
	std::string m_strAnimationName;
	std::vector<FbxMatrix> m_Matrices;
	std::vector<AnimationQuaternionKeyFrame> m_Quarternions;

public:
	AnimationKeyFrames(const std::string& strAnimationName, const unsigned int& numFrames=0);

	// Direct Data Access
	FbxMatrix& GetKeyFrameTransform2(int nKeyFrame) { return m_Matrices[nKeyFrame]; }
	const FbxMatrix& GetKeyFrameTransform2(int nKeyFrame) const { return m_Matrices[nKeyFrame]; }
	inline const std::string& GetAnimationName() const { return m_strAnimationName; }
	inline unsigned int GetFrameCount() const { return m_Matrices.size(); }
	inline std::vector<FbxMatrix>& GetMatrices() { return m_Matrices; }

	// Frames
	void AddKeyFrame(const FbxMatrix& matTransform2);
	const AnimationQuaternionKeyFrame& GetKeyFrameQuaternion(unsigned int nKeyFrame);
	void GenerateQuarternions();
};