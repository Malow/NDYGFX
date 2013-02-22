#include "Decal.h"


Decal::Decal(Vector3 pos)
{
	this->size = 1.0f;
	this->position = pos;
	this->textureResource = NULL;
}

Decal::~Decal()
{
	if(this->textureResource) 
	{
		GetResourceManager()->DeleteTextureResource(this->textureResource);
	}
}
