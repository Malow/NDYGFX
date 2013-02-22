#pragma once

#include <string>

#pragma warning ( push )
#pragma warning ( disable : 4995 )

class Animation
{
	std::string m_strName;
	int m_nKeyFrames;
	float m_fFrameRate;
	float m_fDefaultFrameRate;

public:
	Animation(const std::string& strName, int nKeyFrames, float fDefaultFrameRate);
	
	inline void SetFrameRate(float fFrameRate)	{ m_fFrameRate = fFrameRate; }
	inline void ResetFrameRate()				{ m_fFrameRate = m_fDefaultFrameRate; }
	inline float GetFrameRate()					{ return m_fFrameRate; }
	inline float GetDefaultFrameRate()			{ return m_fDefaultFrameRate; }
	inline const std::string& GetName() const	{ return m_strName; }
	inline int GetKeyFrames()					{ return m_nKeyFrames; }
};

#pragma warning ( pop )