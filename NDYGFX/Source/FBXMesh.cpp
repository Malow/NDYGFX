#include "FBXMesh.h"




FBXMesh::FBXMesh( D3DXVECTOR3 pos ) : Mesh(pos)
{

}

FBXMesh::~FBXMesh()
{

}




MaloW::Array<MeshStrip*>* FBXMesh::GetStrips()
{
	return NULL;
}

void FBXMesh::Update( float dt )
{
	this->scene->Update(dt);
}

void FBXMesh::Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* shad, ID3D11DeviceContext* devCont)
{
	this->scene->Render(dt, this->worldMatrix, camProj, camView, shad, devCont );
}

bool FBXMesh::LoadFromFile( string file, IBTHFbx* fbx )
{
	this->scene->Init(file.c_str(), fbx);
	return true;
}
