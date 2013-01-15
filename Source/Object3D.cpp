#include "Object3D.h"

Object3D::Object3D(Buffer* verts, Buffer* inds, TextureResource* textureResource, D3D_PRIMITIVE_TOPOLOGY topology)
{
	this->verts = verts;
	this->inds = inds;
	this->textureResource = textureResource;
	this->topology = topology;
}

Object3D::~Object3D()
{
	if(this->textureResource) GetResourceManager()->DeleteTextureResource(this->textureResource);
	//if(this->texture) this->texture->Release(); this->texture = NULL;
	if(this->verts)
		delete this->verts;
	if(this->inds)
		delete this->inds;
}