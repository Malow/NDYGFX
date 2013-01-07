#include "Object3D.h"

Object3D::Object3D(Buffer* verts, Buffer* inds, Texture* texture, D3D_PRIMITIVE_TOPOLOGY topology)
{
	this->verts = verts;
	this->inds = inds;
	this->texture = texture;
	this->topology = topology;
}

Object3D::~Object3D()
{
	if(this->texture) GetResourceManager()->DeleteTexture(this->texture);
	if(this->verts)
		delete this->verts;
	if(this->inds)
		delete this->inds;
}