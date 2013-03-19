#pragma once

#include <map>
#include <vector>
#include <string>

#include "Dictionary.h"
#include "Animation.h"

#include <BTHFbxAnimationController.h>

#pragma warning ( push )
#pragma warning ( disable : 4995 )

class AnimationController : public IBTHFbxAnimationController
{
	// Settings
	int m_nCurrentKeyFrame;
	int m_nNextKeyFrame;
	float m_fKeyFrameFactor;
	bool m_bUseQuaternionKeyFrames;
	BTTANIMATION m_AnimationMode;
	float m_fFrameTimer;
	bool m_IsClone;

	Animation* m_pCurrentAnimation;
	std::map<std::string, Animation*> m_AnimationsStr;
	std::vector<Animation*> m_AnimationsInt;

public:
	AnimationController();
	AnimationController(AnimationController* srcAC);
	virtual ~AnimationController();

	void AddAnimation(Animation* pAnimation);
	bool SetCurrentAnimation(const std::string& strAnimationName);
	bool SetCurrentAnimation(unsigned int nIndex);
	Animation* GetCurrentAnimation();

	int GetAnimationCount();
	Animation* GetAnimation(int nIndex);

	void Update(float fElapsedTime);
	void Play();
	void Pause();
	void Stop();

	virtual void SetCurrentKeyFrame(int nKeyFrame);	
	virtual int GetCurrentKeyFrame()					{ return m_nCurrentKeyFrame; }
	virtual int GetNextKeyFrame()						{ return m_nNextKeyFrame; }
	virtual float GetKeyFrameFactor()					{ return m_fKeyFrameFactor; }

	virtual BTTANIMATION GetAnimationMode()				{ return m_AnimationMode; }

	virtual void UseQuaternionKeyFrames(bool bUse)		{ m_bUseQuaternionKeyFrames = bUse; }
	virtual bool UseQuaternionKeyFrames()				{ return m_bUseQuaternionKeyFrames; }

};

#pragma warning ( pop )