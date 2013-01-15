#include "MeshResource.h"

MeshResource::MeshResource() : zFilePath(""), zMesh(NULL)
{

}
MeshResource::MeshResource(string filePath, MaloW::Array<MeshStrip*>* mesh) : zFilePath(filePath), zMesh(mesh)
{

}

MeshResource::~MeshResource()
{
	while(0 < this->zMesh->size())
	{
		delete this->zMesh->getAndRemove(0);
	}
	delete this->zMesh;
}

