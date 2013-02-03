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
	//this->scene->Update(dt);
}

void FBXMesh::Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView)
{
	//this->scene->Render(dt, this->worldMatrix, camProj, camView);
}

bool FBXMesh::LoadFromFile( string file )
{
	//this->scene->Init(file.c_str(), WTF, WTF, WTF);
	return true;
}
