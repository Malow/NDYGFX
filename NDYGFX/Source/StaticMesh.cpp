#include "StaticMesh.h"


StaticMesh::StaticMesh(D3DXVECTOR3 pos, string billboardFilePath, float distanceToSwapToBillboard) : Mesh(pos, billboardFilePath, distanceToSwapToBillboard)
{

}

StaticMesh::~StaticMesh()
{
	//**TILLMAN TEST**
	//GetResourceManager()->UnloadObjectDataResource(this->filePath.c_str());
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
	//return this->strips;
}