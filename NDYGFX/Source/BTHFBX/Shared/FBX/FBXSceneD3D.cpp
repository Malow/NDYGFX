#include "FBXSceneD3D.h"

FBXSceneD3D::FBXSceneD3D() :
	mFBXScene(NULL)
{
}

FBXSceneD3D::~FBXSceneD3D()
{
	for(std::vector<FBXModelD3D*>::iterator i = mModels.begin(); i != mModels.end(); i++)
	{
		SAFE_DELETE(*i);
	}
}

void FBXSceneD3D::Init(const char* filename, IBTHFbx* bthFBX, ID3D11Device* dev, ID3D11DeviceContext* devCont, 
					   Vector3& minPos, Vector3& maxPos)
{
	zFileName = filename;

	mFBXScene = bthFBX->GetScene(filename, minPos, maxPos);

	for(unsigned int i = 0; i < mFBXScene->GetModelCount(); i++)
	{
		FBXModelD3D* model = new FBXModelD3D(this);
		
		model->Init(mFBXScene->GetModel(i), dev, devCont);
		
		mModels.push_back(model);
	}
}

void FBXSceneD3D::Update(float dt)
{
	// Update Scene
	mFBXScene->UpdateScene(dt, true);
}

void FBXSceneD3D::Render(float dt, D3DXMATRIX& world, D3DXMATRIX& worldInverseTranspose, D3DXMATRIX& camProj, D3DXMATRIX& camView, D3DXMATRIX& camViewProj, Shader* mShader, ID3D11DeviceContext* devCont, const std::set<std::string>& hiddenModels)
{
	devCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if(mFBXScene->GetSkeleton())
	{
		float* bonesArray = (float*)mFBXScene->GetSkeleton()->GetSkinTransforms();
		if( bonesArray )
		{
			int nBoneCount = mFBXScene->GetSkeleton()->GetBoneCount();
			mShader->SetMatrixArray("g_mBonesArray", bonesArray, 0, nBoneCount < BTHFBX_MAXBONES_PER_MESH ? nBoneCount : BTHFBX_MAXBONES_PER_MESH );
		}
	}
	mShader->SetMatrix("gScene", world);
	mShader->SetMatrix("gWorld", world);
	mShader->SetMatrix("gWorldInvTrans", worldInverseTranspose);

	mShader->SetMatrix("gView", camView);
	mShader->SetMatrix("gProj", camProj);
	mShader->SetMatrix("gViewProj", camViewProj);
	mShader->SetMatrix("gWVP", world * camViewProj);
	
	mShader->Apply(0);

	for(unsigned int i = 0; i < mModels.size(); i++)
	{
		if ( !hiddenModels.count(mModels[i]->GetName()) )
		{
			mModels[i]->Render(dt, mShader, mFBXScene->GetSkeleton() != NULL ? true : false, devCont);
		}
	}
}

IBTHFbxAnimationController* FBXSceneD3D::GetAnimationController()
{
	if(!this->mFBXScene)
	{
		return 0;
	}

	return mFBXScene->GetAnimationController();
}

IBTHFbxSkeleton* FBXSceneD3D::GetSkeleton()
{
	if (!this->mFBXScene)
	{
		return NULL;
	}

	return mFBXScene->GetSkeleton();
}

BTHFBX_RAY_BOX_RESULT FBXSceneD3D::RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix)
{
	return mFBXScene->RayVsScene(ray, worldMatrix);
}

void FBXSceneD3D::RenderShadow( float dt, D3DXMATRIX& world, D3DXMATRIX& lightViewProj, Shader* mShader, ID3D11DeviceContext* devCont, D3DXVECTOR3 sunDir)
{
	devCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mShader->SetFloat3("gSunDir", sunDir);

	if(mFBXScene->GetSkeleton())
	{
		float* bonesArray = (float*)mFBXScene->GetSkeleton()->GetSkinTransforms();
		if( bonesArray )
		{
			unsigned int nBoneCount = mFBXScene->GetSkeleton()->GetBoneCount();
			mShader->SetMatrixArray("g_mBonesArray", bonesArray, 0, nBoneCount < BTHFBX_MAXBONES_PER_MESH ? nBoneCount : BTHFBX_MAXBONES_PER_MESH );
		}
	}
	mShader->SetMatrix("g_mScale", world);
	mShader->SetMatrix("gWorld", world);
	mShader->SetMatrix("gViewProj", lightViewProj);

	mShader->Apply(0);

	for(unsigned int i = 0; i < mModels.size(); i++)
	{
		mModels[i]->RenderShadow(dt, mShader, mFBXScene->GetSkeleton() != NULL ? true : false, devCont);
	}
}
