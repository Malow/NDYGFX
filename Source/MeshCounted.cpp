#include "MeshCounted.h"

MeshCounted::MeshCounted() : zFilePath(""), zMesh(NULL)
{

}
MeshCounted::MeshCounted(string filePath, MaloW::Array<MeshStrip*>* mesh) : zFilePath(filePath), zMesh(mesh)
{

}

MeshCounted::~MeshCounted()
{
	while(0 < this->zMesh->size())
	{
		delete this->zMesh->getAndRemove(0);
	}
	delete this->zMesh;
}

