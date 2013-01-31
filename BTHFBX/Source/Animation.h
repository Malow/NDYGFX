#pragma once

#include <string>

#pragma warning(disable : 4995)

class Animation
{
public:
	Animation(const std::string& strName, int nKeyFrames, float fDefaultFrameRate);
	~Animation();
	
	void SetFrameRate(float fFrameRate) { m_fFrameRate = fFrameRate; }
	void ResetFrameRate()	{ m_fFrameRate = m_fDefaultFrameRate; }
	float GetFrameRate()	{ return m_fFrameRate; }
	float GetDefaultFrameRate()			{ return m_fDefaultFrameRate; }
	std::string GetName()				{ return m_strName; }
	int GetKeyFrames()	{ return m_nKeyFrames; }
protected:
	std::string m_strName;
	int m_nKeyFrames;
	float m_fFrameRate;
	float m_fDefaultFrameRate;
};