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
	//m_Animations.RemoveAll(true);
	//m_pCurrentAnimation = NULL;
}

//--------------------------------------------------------------------------------------
void AnimationController::AddAnimation(Animation* pAnimation)
{
	//m_Animations.Add( pAnimation->GetName(), pAnimation );

	m_AnimationsInt.push_back(pAnimation);
	m_AnimationsStr[pAnimation->GetName()] = pAnimation;

}

//--------------------------------------------------------------------------------------
void AnimationController::SetCurrentAnimation(const std::string& strAnimationName)
{
	auto i = m_AnimationsStr.find(strAnimationName);

	if(i != m_AnimationsStr.end())
	{
		m_pCurrentAnimation = i->second;
	}
	else
	{
		m_pCurrentAnimation = NULL;
	}
	/*
	std::pair<int,Animation*> pair = m_Animations.Find( strAnimationName );

	if( pair.first >= 0 )
		m_pCurrentAnimation = pair.second;
	else
		m_pCurrentAnimation = NULL;
	*/

	Stop();
}

//--------------------------------------------------------------------------------------
void AnimationController::SetCurrentAnimation(int nIndex)
{
	//m_pCurrentAnimation = m_Animations.GetDataArray()[nIndex].second;
	m_pCurrentAnimation = m_AnimationsInt[nIndex];
	Stop();
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
	//return (int)m_Animations.GetDataArray().size();
	return (int)m_AnimationsInt.size();
}

//--------------------------------------------------------------------------------------
Animation* AnimationController::GetAnimation(int nIndex)
{
	//return m_Animations.GetDataArray()[nIndex].second;
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