#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include <BTHFbxModel.h>
#include "..\..\..\Array.h"
#include <Vector>

class FBXSceneD3D;
class FBXModelPartD3D;


class FBXModelD3D
{
	// Scene
	FBXSceneD3D* zScene;

	// Real Data
	IBTHFbxModel*				mBTHFBXModel;

	// Properties
	D3DXMATRIX					mGeometricOffset;
	D3DXMATRIX					mAnimationTransform;
	std::string					mName;

	// Parts
	std::vector<FBXModelPartD3D*> mParts;

public:
	FBXModelD3D(FBXSceneD3D* scene);
	~FBXModelD3D();

	void Init(IBTHFbxModel* model, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Render(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont);
	void RenderShadow(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont);

	const D3DXMATRIX& GetGeometricOffset();
	const D3DXMATRIX& GetAnimationTransform();

	inline const char* GetName() { return mName.c_str(); }
	inline FBXSceneD3D* GetScene() { return zScene; }
};