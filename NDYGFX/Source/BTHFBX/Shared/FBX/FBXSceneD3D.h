#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include "FBXModelD3D.h"
#include <BTHFbx.h>


class FBXSceneD3D
{
	// BTHFBX Stuff
	IBTHFbxScene*				mFBXScene;
	IBTHFbxAnimationController*	mFBXAnimationController;
	IBTHFbxSkeleton*			mFBXSkeleton;

	// Properties
	std::string					zFileName;

	// Settings
	bool						mEnableAnimation;

	// Models
	std::vector<FBXModelD3D*>	mModels;

public:
	FBXSceneD3D();
	~FBXSceneD3D();

	void Init(const char* filename, IBTHFbx* bthFBX, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Update(float dt);
	void Render(float dt, D3DXMATRIX world, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* mShader, ID3D11DeviceContext* devCont);

	IBTHFbxAnimationController*	GetAnimationController();
	IBTHFbxSkeleton*			GetSkeleton();

	BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);

	// File Name Of Scene
	inline const std::string& GetFileName() const { return zFileName; }
};