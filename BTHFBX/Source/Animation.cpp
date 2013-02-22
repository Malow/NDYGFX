#include "Animation.h"


Animation::Animation(const std::string& strName, int nKeyFrames, float fDefaultFrameRate) :
	m_strName(strName),
	m_nKeyFrames(nKeyFrames),
	m_fDefaultFrameRate(fDefaultFrameRate),
	m_fFrameRate(fDefaultFrameRate)
{
}