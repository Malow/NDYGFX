#include "FBXModelPartD3D.h"
#include "FBXModelD3D.h"
#include "..\Helpers\BTHResourceManager.h"
#include "FBXModelPartDataD3D.h"


FBXModelPartD3D::FBXModelPartD3D() :
	mDiffuseTexture(0),
	mNormalTexture(0)
{

}

FBXModelPartD3D::~FBXModelPartD3D()
{
	// Model Part
	FBXModelPartDataD3DManager::GetInstance()->FreeModelData(mModelData);

	// Textures
	if ( mDiffuseTexture ) BTHResourceManager::GetInstance()->FreeTexture(mDiffuseTexture);
	if ( mNormalTexture ) BTHResourceManager::GetInstance()->FreeTexture(mNormalTexture);
}

void FBXModelPartD3D::Init( FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	// Parent
	mParentModel = parentModel;

	// Is The ModelPart Skinned?
	m_bSkinnedModel = modelPart->IsSkinnedModel();

	// Load Model Data
	mModelData = FBXModelPartDataD3DManager::GetInstance()->GetModelData(parentModel, modelPart, partIndex, dev, devCont);

	// Default Data
	mVB_Position = mModelData->mVB_Position;
	mVB_Normal = mModelData->mVB_Normal;
	mVB_Tangent = mModelData->mVB_Tangent;
	mVB_TexCoord = mModelData->mVB_TexCoord;
	mVB_BlendWeights = mModelData->mVB_BlendWeights;
	mIB = mModelData->mIB;
}

void FBXModelPartD3D::Render(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont)
{
	// Diffuse Texture
	if ( mDiffuseTexture )
	{
		shader->SetResource("txDiffuse", mDiffuseTexture->GetResource());
	}
	else if ( mModelData && mModelData->mDiffuseTexture )
	{
		shader->SetResource("txDiffuse", mModelData->mDiffuseTexture? mModelData->mDiffuseTexture->GetResource() : NULL);
	}

	// Normal Texture
	if ( mNormalTexture )
	{
		shader->SetResource("txNormal", mNormalTexture->GetResource());
	}
	else if ( mModelData && mModelData->mNormalTexture )
	{
		shader->SetResource("txNormal", mModelData->mNormalTexture? mModelData->mNormalTexture->GetResource() : NULL);
	}

	// Skinning Property
	shader->SetBool("g_bSkinning", enableAnimation ? m_bSkinnedModel : false);
	
	// World Matrix
	if( enableAnimation && !m_bSkinnedModel )
	{
		D3DXMATRIX temp = mParentModel->GetGeometricOffset() * mParentModel->GetAnimationTransform();
		shader->SetMatrix("gWorld", temp);
	}
	else
	{
		D3DXMATRIX temp = mParentModel->GetGeometricOffset();
		shader->SetMatrix("gWorld", temp);
	}

	// Buffers
	ID3D11Buffer* aVB[5] = { 
		mVB_Position->GetBufferPointer(), 
		mVB_Normal->GetBufferPointer(), 
		mVB_Tangent->GetBufferPointer(), 
		mVB_TexCoord->GetBufferPointer(), 
		(mVB_BlendWeights!=0? mVB_BlendWeights->GetBufferPointer() : 0 ) 
	};

	// Sizes Of Buffers
	static const UINT aStrides[5] = { 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR2), 
		sizeof(BTHFBX_BLEND_WEIGHT_DATA) 
	};

	static const UINT aOffsets[5] = {0, 0, 0, 0, 0};

	devCont->IASetVertexBuffers(0, 4+(mVB_BlendWeights!=0), aVB, aStrides, aOffsets);
	devCont->IASetIndexBuffer(mIB->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
	shader->Apply(0);
	devCont->DrawIndexed((unsigned int)mIB->GetElementCount(), 0, 0);
}




void FBXModelPartD3D::RenderShadow(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont)
{
	shader->SetBool("g_bSkinning", enableAnimation ? m_bSkinnedModel : false);

	if( enableAnimation && !m_bSkinnedModel )
	{
		D3DXMATRIX temp = mParentModel->GetGeometricOffset() * mParentModel->GetAnimationTransform();
		shader->SetMatrix("gWorld", temp);
	}
	else
	{
		D3DXMATRIX temp = mParentModel->GetGeometricOffset();
		shader->SetMatrix("gWorld", temp);
	}

	// Buffers
	ID3D11Buffer* aVB[5] = { 
		mVB_Position->GetBufferPointer(), 
		mVB_Normal->GetBufferPointer(), 
		mVB_Tangent->GetBufferPointer(), 
		mVB_TexCoord->GetBufferPointer(), 
		(mVB_BlendWeights!=0? mVB_BlendWeights->GetBufferPointer() : 0 ) 
	};

	// Sizes Of Buffers
	static const UINT aStrides[5] = { 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR2), 
		sizeof(BTHFBX_BLEND_WEIGHT_DATA) 
	};

	static const UINT aOffsets[5] = {0, 0, 0, 0, 0};

	devCont->IASetVertexBuffers(0, 4+(mVB_BlendWeights!=0), aVB, aStrides, aOffsets);
	devCont->IASetIndexBuffer(mIB->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
	shader->Apply(0);
	devCont->DrawIndexed((unsigned int)mIB->GetElementCount(), 0, 0);
}