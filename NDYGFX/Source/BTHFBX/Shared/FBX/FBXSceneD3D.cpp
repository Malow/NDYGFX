#include "FBXSceneD3D.h"

FBXSceneD3D::FBXSceneD3D()
{
	mFBXAnimationController = NULL;
	mFBXSkeleton = NULL;
}

FBXSceneD3D::~FBXSceneD3D()
{
	for(std::vector<FBXModelD3D*>::iterator i = mModels.begin(); i != mModels.end(); i++)
	{
		SAFE_DELETE(*i);
	}
}

void FBXSceneD3D::Init(const char* filename, IBTHFbx* bthFBX, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	mFBXScene = bthFBX->GetScene(filename);
	if(mFBXScene)
	{
		mFBXAnimationController = mFBXScene->GetAnimationController();
		mFBXSkeleton = mFBXScene->GetSkeleton();
	}


	for(int i = 0; i < mFBXScene->GetModelCount(); i++)
	{
		FBXModelD3D* model = new FBXModelD3D();
		
		model->Init(mFBXScene->GetModel(i), dev, devCont);
		
		mModels.push_back(model);
	}
}

void FBXSceneD3D::Update(float dt)
{
	for(int i = 0; i < (int)mModels.size(); i++)
	{
		mModels[i]->Update(dt);
	}
}

void FBXSceneD3D::Render(float dt, D3DXMATRIX world, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* mShader, ID3D11DeviceContext* devCont)
{
	devCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if(mFBXSkeleton)
	{
		float* bonesArray = (float*)mFBXSkeleton->GetSkinTransforms();
		if( bonesArray )
		{
			int nBoneCount = mFBXSkeleton->GetBoneCount();

			mShader->SetMatrixArray("g_mBonesArray", bonesArray, 0, nBoneCount < BTHFBX_MAXBONES_PER_MESH ? nBoneCount : BTHFBX_MAXBONES_PER_MESH );

			// ??????????
			D3DXMATRIX mat;
			for(int i = 0; i < nBoneCount; i++)
			{
				memcpy(&mat, bonesArray + i * 16, sizeof(D3DXMATRIX));
			}
		}
	}
	mShader->SetMatrix("g_mScale", world);
	mShader->SetMatrix("gWorld", world);

	D3DXMATRIX vp = camView * camProj;

	mShader->SetMatrix("gView", camView);
	mShader->SetMatrix("gProj", camProj);
	mShader->SetMatrix("gViewProj", vp);
	mShader->SetMatrix("gWVP", world * vp);
	
	mShader->Apply(0);

	for(int i = 0; i < (int)mModels.size(); i++)
	{
		mModels[i]->Render(dt, mShader, vp, mFBXSkeleton != NULL ? true : false, devCont);
	}
}

IBTHFbxAnimationController* FBXSceneD3D::GetAnimationController()
{
	return mFBXAnimationController;
}

IBTHFbxSkeleton* FBXSceneD3D::GetSkeleton()
{
	return mFBXSkeleton;
}

BTHFBX_RAY_BOX_RESULT FBXSceneD3D::RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix)
{
	return mFBXScene->RayVsScene(ray, worldMatrix);
}