#include "MeshStripsResource.h"

MeshStripsResource::MeshStripsResource() : zFilePath(""), zMesh(NULL)
{

}
MeshStripsResource::MeshStripsResource(string filePath, MaloW::Array<MeshStrip*>* mesh) : zFilePath(filePath), zMesh(mesh)
{

}
MeshStripsResource::MeshStripsResource(const MeshStripsResource& origObj)
{
	this->zFilePath = origObj.zFilePath;

	while(0 < this->zMesh->size())
	{
		delete this->zMesh->getAndRemove(0);
	}
	delete this->zMesh;
	this->zMesh = new MaloW::Array<MeshStrip*>();
	for(unsigned int i = 0; i < origObj.zMesh->size(); i++)
	{
		this->zMesh->add(origObj.zMesh->get(i)); 
	}
}

MeshStripsResource::~MeshStripsResource()
{
	while(0 < this->zMesh->size())
	{
		delete this->zMesh->getAndRemove(0);
	}
	delete this->zMesh;
}

