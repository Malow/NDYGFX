#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"

extern "C"
{
	class DECLDIR iAnimatedMesh : public virtual iMesh
	{	
	protected:
		iAnimatedMesh() {};
		virtual ~iAnimatedMesh() {};

	public:
		virtual unsigned int	GetNrOfTimesLooped()	const = 0;
		virtual bool			IsLooping()				const = 0;
		virtual bool			IsLoopingNormal()		const = 0;
		virtual bool			IsLoopingSeamless()		const = 0;
		virtual unsigned int	GetAnimationLength()	const = 0;
		
		/* Sets the time to animated with. */
		virtual void SetAnimationTime(float animationTime) = 0;

		/*! Prevents looping. */
		virtual void NoLooping() = 0;
		/*! Loops by returning to the first keyframe when last keyframe is reached. Note that this kind of looping is not seamless. */
		virtual void LoopNormal() = 0;
		/*! Loops by adding the first keyframe as the last keyframe to prevent seamed(normal) looping */
		virtual void LoopSeamless() = 0; 

	};
}