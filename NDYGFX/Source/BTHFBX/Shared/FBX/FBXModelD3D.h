#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include "..\..\Include\BTHFbxModel.h"
#include "FBXModelPartD3D.h"
#include <stdlib.h>


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

	void Init(IBTHFbxModel* model);

	void Update(float dt);
	void Render(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation);

	const D3DXMATRIX& GetGeometricOffset();
	const D3DXMATRIX& GetAnimationTransform();

	const char* GetName() { return mName.c_str(); }
};