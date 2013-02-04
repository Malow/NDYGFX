#include "FBXModelPartD3D.h"

#include "FBXModelD3D.h"

#include "..\Source\BTHFBX\Shared\Helpers\ResourceManager.h"

#include "FBXModelPartDataD3D.h"

FBXModelPartD3D::FBXModelPartD3D()
{

}

FBXModelPartD3D::~FBXModelPartD3D()
{

}

void FBXModelPartD3D::Init( FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex)
{
	mParentModel = parentModel;

	m_bSkinnedModel = modelPart->IsSkinnedModel();

	mModelData = FBXModelPartDataD3DManager::GetInstance()->GetModelData(parentModel, modelPart, partIndex);

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
	#define EFFECTVARIABLENAME_WORLDMATRIX			"gWorld"
	#define EFFECTVARIABLENAME_DIFFUSETEXTURE		"txDiffuse"
	#define EFFECTVARIABLENAME_NORMALTEXTURE		"txNormal"
	#define EFFECTVARIABLENAME_SKINNING				"g_bSkinning"

	shader->SetResource(EFFECTVARIABLENAME_DIFFUSETEXTURE, mDiffuseTexture ? mDiffuseTexture->GetResource() : NULL);
	shader->SetResource(EFFECTVARIABLENAME_NORMALTEXTURE, mNormalTexture ? mNormalTexture->GetResource() : NULL);
	shader->SetBool(EFFECTVARIABLENAME_SKINNING, enableAnimation ? m_bSkinnedModel : false);
	
	if( enableAnimation )
	{
		if( m_bSkinnedModel )	
		{
			D3DXMATRIX temp = mParentModel->GetGeometricOffset();
			shader->SetMatrix(EFFECTVARIABLENAME_WORLDMATRIX, temp );
		}
		else	
		{
			D3DXMATRIX temp = mParentModel->GetGeometricOffset() * mParentModel->GetAnimationTransform();
			shader->SetMatrix(EFFECTVARIABLENAME_WORLDMATRIX, temp);
		}
	}
	else
	{
		D3DXMATRIX temp = mParentModel->GetGeometricOffset();
		shader->SetMatrix(EFFECTVARIABLENAME_WORLDMATRIX, temp );
	}


	ID3D11Buffer* aVB[5] = { mVB_Position->GetBufferPointer(), mVB_Normal->GetBufferPointer(), mVB_Tangent->GetBufferPointer(), mVB_TexCoord->GetBufferPointer(), mVB_BlendWeights->GetBufferPointer() };
	UINT aStrides[5] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(BTHFBX_BLEND_WEIGHT_DATA) };
	UINT aOffsets[5] = {0, 0, 0, 0, 0};

	devCont->IASetVertexBuffers( 0, 5, aVB, aStrides, aOffsets );
	devCont->IASetIndexBuffer( mIB->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0 );

	//for(unsigned int pass = 0; pass < shader->PassCount(); pass++)
	//{
		//shader->Apply(pass);
		devCont->DrawIndexed((unsigned int)mIB->GetElementCount(),0,0);
	//}
}