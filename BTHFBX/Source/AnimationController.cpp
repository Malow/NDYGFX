#include "AnimationController.h"

//--------------------------------------------------------------------------------------
AnimationController::AnimationController()
{
	m_pCurrentAnimation = NULL;
	m_nCurrentKeyFrame = 0;
	m_nNextKeyFrame = 0;
	m_fKeyFrameFactor = 0.0f;
	m_fFrameTimer = 0.0f;

	m_bUseQuaternionKeyFrames = true;

	m_IsClone = false;

	m_AnimationMode = BTTANIMATION_STOP;
}

AnimationController::AnimationController(AnimationController* srcAC)
{
	m_pCurrentAnimation = NULL;
	m_nCurrentKeyFrame = 0;
	m_nNextKeyFrame = 0;
	m_fKeyFrameFactor = 0.0f;
	m_fFrameTimer = 0.0f;
	m_bUseQuaternionKeyFrames = true;
	m_IsClone = true;
	m_AnimationMode = BTTANIMATION_STOP;
	m_AnimationsStr = srcAC->m_AnimationsStr;
	m_AnimationsInt = srcAC->m_AnimationsInt;
}

//--------------------------------------------------------------------------------------
AnimationController::~AnimationController()
{
	if(!m_IsClone)
	{
		for(auto i = m_AnimationsInt.begin(); i != m_AnimationsInt.end(); i++)
		{
			if ( *i )
			{
				delete *i;
			}
		}
	}
}

//--------------------------------------------------------------------------------------
void AnimationController::AddAnimation(Animation* pAnimation)
{
	m_AnimationsInt.push_back(pAnimation);
	m_AnimationsStr[pAnimation->GetName()] = pAnimation;
}

//--------------------------------------------------------------------------------------
bool AnimationController::SetCurrentAnimation(const std::string& strAnimationName)
{
	// Find Animation
	auto i = m_AnimationsStr.find(strAnimationName);

	// Check If Found
	if(i != m_AnimationsStr.end())
	{
		m_pCurrentAnimation = i->second;
		SetCurrentKeyFrame(0);
		return true;
	}
	else
	{
		m_pCurrentAnimation = NULL;
		return false;
	}
}

//--------------------------------------------------------------------------------------
bool AnimationController::SetCurrentAnimation(unsigned int nIndex)
{
	if ( nIndex >= m_AnimationsInt.size() ) return false;
	return SetCurrentAnimation(m_AnimationsInt[nIndex]->GetName());
}

//--------------------------------------------------------------------------------------
Animation* AnimationController::GetCurrentAnimation()
{
	if( m_pCurrentAnimation )
		return m_pCurrentAnimation;

	return NULL;
}

//--------------------------------------------------------------------------------------
int AnimationController::GetAnimationCount()
{
	return (int)m_AnimationsInt.size();
}

//--------------------------------------------------------------------------------------
Animation* AnimationController::GetAnimation(int nIndex)
{
	return m_AnimationsInt[nIndex];
}

//--------------------------------------------------------------------------------------
void AnimationController::Update(float fElapsedTime)
{
	if( !m_pCurrentAnimation || m_AnimationMode != BTTANIMATION_PLAY )
		return;

	m_fFrameTimer += fElapsedTime;

	float fInvFrameRate = 1.0f/m_pCurrentAnimation->GetFrameRate();
	while( m_fFrameTimer > fInvFrameRate )
	{
		m_nCurrentKeyFrame = (m_nCurrentKeyFrame+1)%m_pCurrentAnimation->GetKeyFrames();
		m_nNextKeyFrame = (m_nCurrentKeyFrame+1)%m_pCurrentAnimation->GetKeyFrames();
		m_fFrameTimer -= fInvFrameRate;
	}

	m_fKeyFrameFactor = m_fFrameTimer/fInvFrameRate;
}

//-------------------------------------------------------------------------------------- 
void AnimationController::Play()
{
	m_AnimationMode = BTTANIMATION_PLAY;
}

//--------------------------------------------------------------------------------------
void AnimationController::Pause()
{
	m_AnimationMode = BTTANIMATION_PAUSE;
}

//--------------------------------------------------------------------------------------
void AnimationController::Stop()
{
	m_AnimationMode = BTTANIMATION_STOP;
	m_fFrameTimer = 0.0f;
	SetCurrentKeyFrame(0);
}

//--------------------------------------------------------------------------------------
void AnimationController::SetCurrentKeyFrame(int nKeyFrame)
{
	m_nCurrentKeyFrame = (nKeyFrame)%m_pCurrentAnimation->GetKeyFrames();
	m_nNextKeyFrame = (m_nCurrentKeyFrame+1)%m_pCurrentAnimation->GetKeyFrames();
	m_fKeyFrameFactor = 0.0f;
}