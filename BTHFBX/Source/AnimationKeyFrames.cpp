#include "AnimationKeyFrames.h"


AnimationKeyFrames::AnimationKeyFrames(const std::string& strAnimationName, const unsigned int& numFrames)
{
	m_strAnimationName = strAnimationName;
	m_Matrices.resize(numFrames);
}

void AnimationKeyFrames::AddKeyFrame(const FbxMatrix& matTransform2)
{
	m_Matrices.push_back(matTransform2);
	m_Quarternions.push_back(AnimationQuaternionKeyFrame(matTransform2));
}

const AnimationQuaternionKeyFrame& AnimationKeyFrames::GetKeyFrameQuaternion(unsigned int nKeyFrame)
{
	return m_Quarternions[nKeyFrame];
}

void AnimationKeyFrames::GenerateQuarternions()
{
	m_Quarternions.resize(m_Matrices.size());
	for( unsigned int x=0; x<m_Matrices.size(); ++x )
	{
		m_Quarternions[x] = AnimationQuaternionKeyFrame(m_Matrices[x]);
	}
}