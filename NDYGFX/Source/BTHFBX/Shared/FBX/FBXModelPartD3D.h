#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include "..\D3D\BTHTexture.h"
#include <BTHFbxModelPart.h>

class FBXModelPartD3D
{
	class FBXModelPartDataD3D*	mModelData;

	//// TEMP
	Buffer*						mVB_Position;
	Buffer*						mVB_Normal;
	Buffer*						mVB_Tangent;
	Buffer*						mVB_TexCoord;
	Buffer*						mVB_BlendWeights;
	Buffer*						mIB;

	BTHTexture*					mDiffuseTexture;
	BTHTexture*					mNormalTexture;
	////

	bool						m_bSkinnedModel;

	class FBXModelD3D*			mParentModel;
public:
	FBXModelPartD3D();
	~FBXModelPartD3D();

	void Init(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Update(float dt);
	void Render(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation, ID3D11DeviceContext* devCont);
};