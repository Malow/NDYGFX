#include "StaticMesh.h"


StaticMesh::StaticMesh(D3DXVECTOR3 pos, string billboardFilePath, float distanceToSwapToBillboard) : Mesh(pos, billboardFilePath, distanceToSwapToBillboard)
{

}

StaticMesh::~StaticMesh()
{

}

bool StaticMesh::LoadFromFile(string file)
{
	return Mesh::LoadFromFile(file);
}

MaloW::Array<MeshStrip*>* StaticMesh::GetStrips()
{ 
	if(this->meshStripsResource != NULL)
	{
		return this->meshStripsResource->GetMeshStripsPointer(); 
	}
	else
	{
		return NULL;
	}
}