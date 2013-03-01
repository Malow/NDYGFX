#pragma once

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include "..\D3D\BTHTexture.h"
#include <BTHFbxModelPart.h>

class FBXModelPartDataD3D;
class FBXModelD3D;

class FBXModelPartD3D
{
	FBXModelPartDataD3D*	mModelData;

	//// TEMP
	Buffer*					mVB_Position;
	Buffer*					mVB_Normal;
	Buffer*					mVB_Tangent;
	Buffer*					mVB_TexCoord;
	Buffer*					mVB_BlendWeights;
	Buffer*					mIB;

	BTHTexture*				mDiffuseTexture;
	BTHTexture*				mNormalTexture;

	// Does The Model Bind To A Skeleton
	bool					m_bSkinnedModel;

	// Belongs To Model
	FBXModelD3D*			mParentModel;
public:
	FBXModelPartD3D();
	~FBXModelPartD3D();

	void Init(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Render(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont);
	void RenderShadow(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont);
};