#pragma once

#include "..\..\Include\BTHstdafx.h"
#include "..\..\Include\BTHFbx.h"

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"

#include "FBXModelD3D.h"

struct ShadowData
{
	class BTHCamera* lightCamera;
	ID3D11ShaderResourceView* shadowRV;
};

class FBXSceneD3D
{
	std::vector<FBXModelD3D*>	mModels;


	IBTHFbxScene*				mFBXScene;
	IBTHFbxAnimationController*	mFBXAnimationController;
	IBTHFbxSkeleton*			mFBXSkeleton;

	bool						mEnableAnimation;

public:
	FBXSceneD3D();
	~FBXSceneD3D();

	void Init(const char* filename, IBTHFbx* bthFBX, char* shaderFilename);

	void Update(float dt);
	void Render(float dt, D3DXMATRIX world, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* mShader, ID3D11DeviceContext* devCont);

	IBTHFbxAnimationController*	GetAnimationController();
	IBTHFbxSkeleton*			GetSkeleton();

	BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);
};
