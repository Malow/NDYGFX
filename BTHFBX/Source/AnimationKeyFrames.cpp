#include "AnimationKeyFrames.h"


AnimationKeyFrames::AnimationKeyFrames(const std::string& strAnimationName, const unsigned int& numFrames)
{
	m_strAnimationName = strAnimationName;
	m_Frames.resize(numFrames);
}

AnimationKeyFrames::~AnimationKeyFrames()
{
}

void AnimationKeyFrames::AddKeyFrame(const FbxMatrix& matTransform2)
{
	m_Frames.push_back( std::make_pair( matTransform2, AnimationQuaternionKeyFrame(matTransform2) ) );
}

const AnimationQuaternionKeyFrame& AnimationKeyFrames::GetKeyFrameQuaternion(int nKeyFrame)
{
	return m_Frames.at(nKeyFrame).second;
}