#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include <BTHFbxModel.h>
#include "FBXModelPartD3D.h"
#include "..\..\..\Array.h"
#include <Vector>


class FBXModelD3D
{
	std::vector<FBXModelPartD3D*> mParts;

	D3DXMATRIX					mGeometricOffset;
	D3DXMATRIX					mAnimationTransform;

	IBTHFbxModel*				mBTHFBXModel;

	std::string					mName;

public:
	FBXModelD3D();
	~FBXModelD3D();

	void Init(IBTHFbxModel* model, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Update(float dt);
	void Render(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation, ID3D11DeviceContext* devCont);

	const D3DXMATRIX& GetGeometricOffset();
	const D3DXMATRIX& GetAnimationTransform();

	const char* GetName() { return mName.c_str(); }
};