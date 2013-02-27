#ifndef __bthfbx_scene_h__
#define __bthfbx_scene_h__

#include "BTHFbxCommon.h"

#include "BTHFbxModel.h"
#include "BTHFbxAnimationController.h"
#include "BTHFbxSkeleton.h"
#include "BTHFbxCurve.h"

extern "C"
{
	class DECLDIR IBTHFbxScene
	{
	public:
		virtual bool IsAnimated() = 0;
		virtual bool IsSkinned() = 0;
		virtual IBTHFbxAnimationController* GetAnimationController() = 0;
		virtual IBTHFbxSkeleton* GetSkeleton() = 0;
		
		virtual unsigned int GetModelCount() = 0;
		virtual IBTHFbxModel* GetModel(unsigned int index) = 0;
		
		virtual unsigned int GetCurveCount() = 0;
		virtual IBTHFbxCurve* GetCurve(unsigned int index) = 0;

		virtual BTHFBX_AABB_DATA GetBoundingBoxData() = 0;

		virtual BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix) = 0;
		
		virtual void UpdateScene(float fElapsedTime, bool bEnableAnimation) = 0;

		// DEBUG!!!!!
		//virtual void InitializeDeviceObjects(void* pd3dDeviceContext) = 0;
		//virtual void RenderScene(void* pd3dDeviceContext, void* pBTTEffect) = 0;
		//virtual void RenderSceneDebug(void* pd3dDeviceContext, void* pBTTEffect) = 0;
	};
}

#endif