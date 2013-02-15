#include "Object3D.h"

Object3D::Object3D(BufferResource* verts, BufferResource* inds, TextureResource* textureResource, 
				   TextureResource* normalMap, D3D_PRIMITIVE_TOPOLOGY topology)
{
	this->verts = verts;
	this->inds = inds;
	this->textureResource = textureResource;
	this->topology = topology;
	this->normalMap = normalMap;
}

Object3D::~Object3D()
{
	if(this->textureResource) GetResourceManager()->DeleteTextureResource(this->textureResource); 
	if(this->normalMap) GetResourceManager()->DeleteTextureResource(this->normalMap);
	if(this->verts) GetResourceManager()->DeleteBufferResource(this->verts); 
	if(this->inds) GetResourceManager()->DeleteBufferResource(this->inds);
}

Buffer* Object3D::GetVertBuff()
{
	if(this->verts != NULL)
	{
		return this->verts->GetBufferPointer();
	}
	else
	{
		return NULL;
	}
}
Buffer* Object3D::GetIndsBuff()
{
	if(this->inds != NULL)
	{
		return this->inds->GetBufferPointer();
	}
	else
	{
		return NULL;
	}
}