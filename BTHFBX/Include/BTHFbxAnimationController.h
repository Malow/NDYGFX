#ifndef __bthfbx_animation_controller_h__
#define __bthfbx_animation_controller_h__

#include "BTHFbxCommon.h"

extern "C"
{
	enum BTTANIMATION
	{
		BTTANIMATION_PLAY,
		BTTANIMATION_PAUSE,
		BTTANIMATION_STOP,
	};

	class DECLDIR IBTHFbxAnimationController
	{
	public:
		virtual bool SetCurrentAnimation(const std::string& strAnimationName) = 0;
		virtual bool SetCurrentAnimation(unsigned int nIndex) = 0;

		virtual int GetAnimationCount() = 0;

		virtual void Update(float fElapsedTime) = 0;
		virtual void Play() = 0; 
		virtual void Pause() = 0;
		virtual void Stop() = 0;

		virtual void SetCurrentKeyFrame(int nKeyFrame) = 0;
		virtual int GetCurrentKeyFrame() = 0;
		virtual int GetNextKeyFrame() = 0;
		virtual float GetKeyFrameFactor() = 0;

		virtual BTTANIMATION GetAnimationMode() = 0;

		virtual void UseQuaternionKeyFrames(bool bUse) = 0;
		virtual bool UseQuaternionKeyFrames() = 0;
	};
}

#endif