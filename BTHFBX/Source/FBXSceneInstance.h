#pragma once

#include "FBXSDK.h"

#include "FBXCommon.h"
#include "Model.h"
#include "Skeleton.h"
#include "BoneWeights.h"
#include "AnimationController.h"
#include "Curve.h"
#include <mutex>

#include <BTHFbxScene.h>
#include <BTHFbxSkeleton.h>
#include <BTHFbxAnimationController.h>

class FBXSceneInstance : public IBTHFbxScene
{
	class FBXScene* mScene;
	Skeleton* m_pSkeleton;

	Dictionary<Model*> m_Models;
	std::vector<Curve>* m_Curves;

	std::mutex zControllerMutex;
	AnimationController* m_pAnimationController;
	BTHFBX_AABB_DATA m_BoundingBox;

public:
	FBXSceneInstance(class FBXScene* scene);
	virtual ~FBXSceneInstance();

	void InitInstance();
	void UpdateScene(float fElapsedTime, bool bEnableAnimation);

	virtual unsigned int GetModelCount();
	virtual IBTHFbxModel* GetModel(unsigned int index);
	virtual bool IsAnimated();
	virtual bool IsSkinned();
	virtual IBTHFbxAnimationController* GetAnimationController();	
	virtual IBTHFbxSkeleton* GetSkeleton();
	virtual unsigned int GetCurveCount();
	virtual IBTHFbxCurve* GetCurve(unsigned int index);
	virtual BTHFBX_AABB_DATA GetBoundingBoxData();
	virtual BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);

private:
	bool HasFBXAnimation(FbxNode *pNode);
	void ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attributeType);
	void ProcessSkeleton(FbxNode* pNode);

	FbxMatrix GetAbsoluteTransformFromCurrentTake2(FbxNode* pNode, FbxTime time);
};