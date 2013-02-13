#include "WaterPlane.h"


WaterPlane::WaterPlane(D3DXVECTOR3 pos) : Mesh(pos)
{
	this->verts = NULL;
	this->vertBuffer = NULL;
	this->textureResource = NULL;

	this->verts = new Vertex[6];
	this->verts[0] = Vertex(D3DXVECTOR3(-0.5f, 0.0f, -0.5f), D3DXVECTOR2(0.0f, 1.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[1] = Vertex(D3DXVECTOR3(-0.5f, 0.0f, 0.5f), D3DXVECTOR2(0.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[2] = Vertex(D3DXVECTOR3(0.5f, 0.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	this->verts[3] = Vertex(D3DXVECTOR3(-0.5f, 0.0f, -0.5f), D3DXVECTOR2(0.0f, 1.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[4] = Vertex(D3DXVECTOR3(0.5f, 0.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[5] = Vertex(D3DXVECTOR3(0.5f, 0.0f, -0.5f), D3DXVECTOR2(1.0f, 1.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	this->nrOfVerts = 6;
}

WaterPlane::~WaterPlane()
{
	if(this->verts)
	{
		delete this->verts; 
		this->verts = NULL;
	}
	if(this->vertBuffer) 
	{
		delete this->vertBuffer; 
		this->vertBuffer = NULL;
	}
	if(this->textureResource) 
	{
		GetResourceManager()->DeleteTextureResource(this->textureResource);
	}
	if(this->textureResource2) 
	{
		GetResourceManager()->DeleteTextureResource(this->textureResource2);
	}
}

MaloW::Array<MeshStrip*>* WaterPlane::GetStrips()
{
	return NULL;
}