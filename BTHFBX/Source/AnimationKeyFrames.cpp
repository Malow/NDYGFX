#include "AnimationKeyFrames.h"

//--------------------------------------------------------------------------------------
AnimationKeyFrames::AnimationKeyFrames(const std::string& strAnimationName)
{
	m_strAnimationName = strAnimationName;
}

//--------------------------------------------------------------------------------------
AnimationKeyFrames::~AnimationKeyFrames()
{
}

//--------------------------------------------------------------------------------------
//void AnimationKeyFrames::AddKeyFrame(const D3DXMATRIX& matTransform, const FbxMatrix& matTransform2)
void AnimationKeyFrames::AddKeyFrame(const FbxMatrix& matTransform2)
{
	//m_KeyFrames.push_back( std::make_pair( matTransform, AnimationQuaternionKeyFrame(matTransform, matTransform2) ) );
	m_KeyFrames2.push_back( std::make_pair( matTransform2, AnimationQuaternionKeyFrame(matTransform2) ) );
}

//--------------------------------------------------------------------------------------
std::string AnimationKeyFrames::GetAnimationName()
{
	return m_strAnimationName;
}

//--------------------------------------------------------------------------------------
/*
const D3DXMATRIX& AnimationKeyFrames::GetKeyFrameTransform(int nKeyFrame)
{
	return m_KeyFrames.at(nKeyFrame).first;
}
*/

const FbxMatrix& AnimationKeyFrames::GetKeyFrameTransform2(int nKeyFrame)
{
	return m_KeyFrames2.at(nKeyFrame).first;
}

//--------------------------------------------------------------------------------------
const AnimationQuaternionKeyFrame& AnimationKeyFrames::GetKeyFrameQuaternion(int nKeyFrame)
{
	return m_KeyFrames2.at(nKeyFrame).second;
}