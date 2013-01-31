#include "StaticMesh.h"


StaticMesh::StaticMesh(D3DXVECTOR3 pos) : Mesh(pos)
{

}

StaticMesh::~StaticMesh()
{
	GetResourceManager()->UnloadObjectDataResource(this->filePath.c_str());
}

bool StaticMesh::LoadFromFile(string file)
{
	return Mesh::LoadFromFile(file);
}

MaloW::Array<MeshStrip*>* StaticMesh::GetStrips()
{ 
	return this->strips; 
}