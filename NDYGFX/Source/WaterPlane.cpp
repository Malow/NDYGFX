#include "WaterPlane.h"


WaterPlane::WaterPlane(D3DXVECTOR3 pos, string filePath) : Mesh(pos, filePath)
{
	this->verts = NULL;
	this->vertBuffer = NULL;
	this->texture = NULL;

	this->verts = new Vertex[6];
	this->verts[0] = Vertex(D3DXVECTOR3(0.5f, 0.0f, -0.5f), D3DXVECTOR2(0.0f, 1.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[1] = Vertex(D3DXVECTOR3(0.5f, 0.0f, 0.5f), D3DXVECTOR2(0.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[2] = Vertex(D3DXVECTOR3(-0.5f, 0.0f, -0.5f), D3DXVECTOR2(1.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	this->verts[3] = Vertex(D3DXVECTOR3(-0.5f, 0.0f, -0.5f), D3DXVECTOR2(0.0f, 1.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[4] = Vertex(D3DXVECTOR3(0.5f, 0.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	this->verts[5] = Vertex(D3DXVECTOR3(-0.5f, 0.0f, 0.5f), D3DXVECTOR2(1.0f, 1.0f), 
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	this->nrOfVerts = 6;

	this->VertexDataHasChanged = false;
	this->SetScale(1.0f);
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
	if(this->texture) 
	{
		GetResourceManager()->DeleteTextureResource(this->texture);
	}
}

MaloW::Array<MeshStrip*>* WaterPlane::GetStrips()
{
	return NULL;
}

void WaterPlane::SetVertexPosition( Vector3 pos, int vertexIndex )
{
	switch (vertexIndex)
	{
	case 0:
		this->verts[0].pos = pos;
		break;
	case 1:
		this->verts[1].pos = pos;
		this->verts[4].pos = pos;
		break;
	case 2:
		this->verts[2].pos = pos;
		this->verts[3].pos = pos;
		break;
	case 3:
		this->verts[5].pos = pos;
		break;
	default:
		MaloW::Debug("SetVertexPosition in Waterplane out of index.");
		break;
	}

	// Calculate new normal
	Vector3 newNorm = Vector3(0, 0, 0);
	D3DXVECTOR3 one = this->verts[0].pos - this->verts[5].pos;
	D3DXVECTOR3 two = this->verts[1].pos - this->verts[2].pos;
	Vector3 vone = Vector3(one.x, one.y, one.z);
	Vector3 vtwo = Vector3(two.x, two.y, two.z);
	newNorm = vtwo.GetCrossProduct(vone);
	newNorm.Normalize();
	for(int i = 0; i < 6; i++)
		this->verts[i].normal = newNorm;

	this->VertexDataHasChanged = true;
}
