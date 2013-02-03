#include "FBXModelD3D.h"

FBXModelD3D::FBXModelD3D()
{
	D3DXMatrixIdentity(&mGeometricOffset);
	D3DXMatrixIdentity(&mAnimationTransform);
}

FBXModelD3D::~FBXModelD3D()
{
	for(std::vector<FBXModelPartD3D*>::iterator i = mParts.begin(); i != mParts.end(); i++)
	{
		SAFE_DELETE(*i);
	}
}

void FBXModelD3D::Init(IBTHFbxModel* model, Device3D* device3D)
{
	mDevice3D = device3D;
	mBTHFBXModel = model;

	mName = model->GetName();

	for(int i = 0; i < model->GetModelPartCount(); i++)
	{
		FBXModelPartD3D* part = myNew FBXModelPartD3D();
		
		part->Init(this, model->GetModelPart(i), mDevice3D, i);

		mParts.push_back(part);
	}
}

void FBXModelD3D::Update(float dt)
{

}

void FBXModelD3D::Render(float dt, Shader* shader, D3DXMATRIX viewProj, bool enableAnimation)
{
	mGeometricOffset = *(D3DXMATRIX*)mBTHFBXModel->GetGeometricOffset();
	mAnimationTransform = *(D3DXMATRIX*)mBTHFBXModel->GetAnimationTransform();

	for(int i = 0; i < (int)mParts.size(); i++)
	{
		mParts[i]->Render(dt, shader, viewProj, enableAnimation);
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