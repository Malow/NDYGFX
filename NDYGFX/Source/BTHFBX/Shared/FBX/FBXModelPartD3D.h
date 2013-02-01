#pragma once

#include "..\..\Include\BTHstdafx.h"
#include "..\..\Include\BTHFbx.h"

#include "..\..\..\..\Source\Shader.h"
#include "..\..\..\..\Source\Buffer.h"
#include "..\D3D\BTHTexture.h"
//#include "..\Source\BTHFBX\Shared\D3D\Device3D.h"

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

	Texture*					mDiffuseTexture;
	Texture*					mNormalTexture;
	////

	bool						m_bSkinnedModel;

	class FBXModelD3D*			mParentModel;
public:
	FBXModelPartD3D();
	~FBXModelPartD3D();

	void Init(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex);

	void Update(float dt);
	void Render(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation);
};