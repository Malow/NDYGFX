#include "FBXModelPartD3D.h"
#include "FBXModelD3D.h"
#include "..\Helpers\BTHResourceManager.h"
#include "FBXModelPartDataD3D.h"


FBXModelPartD3D::FBXModelPartD3D()
{

}

FBXModelPartD3D::~FBXModelPartD3D()
{

}

void FBXModelPartD3D::Init( FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	mParentModel = parentModel;

	m_bSkinnedModel = modelPart->IsSkinnedModel();

	mModelData = FBXModelPartDataD3DManager::GetInstance()->GetModelData(parentModel, modelPart, partIndex, dev, devCont);

	mVB_Position = mModelData->mVB_Position;
	mVB_Normal = mModelData->mVB_Normal;
	mVB_Tangent = mModelData->mVB_Tangent;
	mVB_TexCoord = mModelData->mVB_TexCoord;
	mVB_BlendWeights = mModelData->mVB_BlendWeights;
	mIB = mModelData->mIB;

	mDiffuseTexture = mModelData->mDiffuseTexture;
	mNormalTexture = mModelData->mNormalTexture;
}

void FBXModelPartD3D::Update(float dt)
{

}

void FBXModelPartD3D::Render(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation, ID3D11DeviceContext* devCont)
{
	shader->SetResource("txDiffuse", mDiffuseTexture ? mDiffuseTexture->GetResource() : NULL);
	shader->SetResource("txNormal", mNormalTexture ? mNormalTexture->GetResource() : NULL);
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
	UINT aStrides[5] = { 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR2), 
		sizeof(BTHFBX_BLEND_WEIGHT_DATA) 
	};

	UINT aOffsets[5] = {0, 0, 0, 0, 0};

	devCont->IASetVertexBuffers(0, 4+(mVB_BlendWeights!=0), aVB, aStrides, aOffsets);
	devCont->IASetIndexBuffer(mIB->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
	shader->Apply(0);
	devCont->DrawIndexed((unsigned int)mIB->GetElementCount(), 0, 0);
}




void FBXModelPartD3D::RenderShadow(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation, ID3D11DeviceContext* devCont)
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
	UINT aStrides[5] = { 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR3), 
		sizeof(D3DXVECTOR2), 
		sizeof(BTHFBX_BLEND_WEIGHT_DATA) 
	};

	UINT aOffsets[5] = {0, 0, 0, 0, 0};

	devCont->IASetVertexBuffers(0, 4+(mVB_BlendWeights!=0), aVB, aStrides, aOffsets);
	devCont->IASetIndexBuffer(mIB->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
	shader->Apply(0);
	devCont->DrawIndexed((unsigned int)mIB->GetElementCount(), 0, 0);
}