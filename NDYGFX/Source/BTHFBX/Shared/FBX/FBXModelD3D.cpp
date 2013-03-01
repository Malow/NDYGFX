#include "FBXModelD3D.h"
#include "FBXSceneD3D.h"
#include "FBXModelPartD3D.h"


FBXModelD3D::FBXModelD3D(FBXSceneD3D* scene) : 
	zScene(scene)
{
	D3DXMatrixIdentity(&mGeometricOffset);
	D3DXMatrixIdentity(&mAnimationTransform);
}

FBXModelD3D::~FBXModelD3D()
{
	for(std::vector<FBXModelPartD3D*>::iterator i = mParts.begin(); i != mParts.end(); ++i)
	{
		SAFE_DELETE(*i);
	}
}

void FBXModelD3D::Init(IBTHFbxModel* model, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	mBTHFBXModel = model;

	mName = model->GetName();

	for(unsigned int i = 0; i < model->GetModelPartCount(); i++)
	{
		FBXModelPartD3D* part = new FBXModelPartD3D();
		
		part->Init(this, model->GetModelPart(i), i, dev, devCont);

		mParts.push_back(part);
	}
}

void FBXModelD3D::Render(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont)
{
	mGeometricOffset = *(D3DXMATRIX*)mBTHFBXModel->GetGeometricOffset();
	mAnimationTransform = *(D3DXMATRIX*)mBTHFBXModel->GetAnimationTransform();

	for(unsigned int i = 0; i < mParts.size(); i++)
	{
		mParts[i]->Render(dt, shader, enableAnimation, devCont);
	}
}

const D3DXMATRIX& FBXModelD3D::GetGeometricOffset()
{
	return mGeometricOffset;
}

const D3DXMATRIX& FBXModelD3D::GetAnimationTransform()
{
	return mAnimationTransform;
}

void FBXModelD3D::RenderShadow(float dt, Shader* shader, bool enableAnimation, ID3D11DeviceContext* devCont)
{
	mGeometricOffset = *(D3DXMATRIX*)mBTHFBXModel->GetGeometricOffset();
	mAnimationTransform = *(D3DXMATRIX*)mBTHFBXModel->GetAnimationTransform();

	for(unsigned int i = 0; i < mParts.size(); i++)
	{
		mParts[i]->RenderShadow(dt, shader, enableAnimation, devCont);
	}
}