#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include "FBXModelD3D.h"
#include <BTHFbx.h>
#include "Vector.h"
#include <set>


class FBXSceneD3D
{
	// BTHFBX Stuff
	IBTHFbxScene*				mFBXScene;

	// Properties
	std::string					zFileName;

	// Settings
	bool						mEnableAnimation;

	// Models
	std::vector<FBXModelD3D*>	mModels;

public:
	FBXSceneD3D();
	~FBXSceneD3D();

	void Init(const char* filename, IBTHFbx* bthFBX, ID3D11Device* dev, ID3D11DeviceContext* devCont, 
		Vector3& minPos, Vector3& maxPos);

	void Update(float dt);
	void Render(float dt, D3DXMATRIX& world, D3DXMATRIX& worldInverseTranspose, D3DXMATRIX& camProj, D3DXMATRIX& camView, D3DXMATRIX& camViewProj, Shader* mShader, ID3D11DeviceContext* devCont, const std::set<std::string>& hiddenModels);
	void RenderShadow(float dt, D3DXMATRIX& world, D3DXMATRIX& lightViewProj, Shader* mShader, ID3D11DeviceContext* devCont, D3DXVECTOR3 sunDir);

	IBTHFbxAnimationController*	GetAnimationController();
	IBTHFbxSkeleton*			GetSkeleton();

	BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);

	// File Name Of Scene
	inline const std::string& GetFileName() const { return zFileName; }
};