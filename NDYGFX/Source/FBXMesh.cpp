#include "FBXMesh.h"




FBXMesh::FBXMesh( D3DXVECTOR3 pos ) : Mesh(pos)
{
	this->scene = new FBXSceneD3D();
}

FBXMesh::~FBXMesh()
{
	if(this->scene)
		delete this->scene;
}




MaloW::Array<MeshStrip*>* FBXMesh::GetStrips()
{
	return NULL;
}

void FBXMesh::Update( float dt )
{
	this->scene->Update(0);
}

void FBXMesh::Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* shad, ID3D11DeviceContext* devCont)
{
	this->RecreateWorldMatrix();
	D3DXMATRIX world = this->GetWorldMatrix();
	this->scene->Render(0, world, camProj, camView, shad, devCont );
}

bool FBXMesh::LoadFromFile( string file, IBTHFbx* fbx, ID3D11Device* dev, ID3D11DeviceContext* devCont )
{
	this->scene->Init(file.c_str(), fbx, dev, devCont);
	this->scene->GetAnimationController()->SetCurrentAnimation(0);
	this->scene->GetAnimationController()->Play();
	return true;
}

void FBXMesh::SetAnimation( int ani )
{
	if(ani < this->scene->GetAnimationController()->GetAnimationCount())
	{
		this->scene->GetAnimationController()->SetCurrentAnimation(ani);
		this->scene->GetAnimationController()->Play();
	}
	else
	{
		MaloW::Debug("Tried to apply animation at index " + MaloW::convertNrToString(ani) + " to an FBX mesh that only has "	+ 
			MaloW::convertNrToString(this->scene->GetAnimationController()->GetAnimationCount()) + " animations.");
	}
}
