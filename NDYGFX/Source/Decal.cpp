#include "Decal.h"


Decal::Decal(Vector3 pos, Vector3 dir, Vector3 up)
{
	this->opa = 1.0f;
	this->dir = dir;
	this->up = up;
	this->size = 1.0f;
	this->position = pos;
	MeshStrip* strip = new MeshStrip();

	// Create sphere
	int nrOfVerts = 8;
	VertexNormalMap *vertices = new VertexNormalMap[nrOfVerts];
	vertices[0] = VertexNormalMap(D3DXVECTOR3(-0.5f, -0.5f, -0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[1] = VertexNormalMap(D3DXVECTOR3(0.5f, -0.5f, -0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[2] = VertexNormalMap(D3DXVECTOR3(-0.5f, -0.5f, 0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[3] = VertexNormalMap(D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[4] = VertexNormalMap(D3DXVECTOR3(-0.5f, 0.5f, -0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[5] = VertexNormalMap(D3DXVECTOR3(0.5f, 0.5f, -0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[6] = VertexNormalMap(D3DXVECTOR3(-0.5f, 0.5f, 0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	vertices[7] = VertexNormalMap(D3DXVECTOR3(0.5f, 0.5f, 0.5f), D3DXVECTOR2(0, 0), D3DXVECTOR3(1, 1, 1));
	
	strip->SetVerts(vertices);
	strip->setNrOfVerts(nrOfVerts);


	int nrOfIndicies = 36;
	int *indices = new int[nrOfIndicies];
	
	// Bottom
	indices[0] = 1;
	indices[1] = 0;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;


	// Left ***********
	indices[6] = 1;
	indices[7] = 5;
	indices[8] = 0;
	indices[9] = 0;
	indices[10] = 5;
	indices[11] = 4;

	// Right
	indices[12] = 2;
	indices[13] = 6;
	indices[14] = 3;
	indices[15] = 3;
	indices[16] = 6;
	indices[17] = 7;

	// Front
	indices[18] = 0;
	indices[19] = 4;
	indices[20] = 2;
	indices[21] = 2;
	indices[22] = 4;
	indices[23] = 6;

	// Back
	indices[24] = 3;
	indices[25] = 7;
	indices[26] = 1;
	indices[27] = 1;
	indices[28] = 7;
	indices[29] = 5;

	// Top
	indices[30] = 4;
	indices[31] = 5;
	indices[32] = 6;
	indices[33] = 6;
	indices[34] = 5;
	indices[35] = 7;

	strip->setNrOfIndicies(nrOfIndicies);
	strip->SetIndicies(indices);
	

	this->strip = strip;
}

Decal::~Decal()
{
	if(this->strip)
	{
		delete this->strip; 
		this->strip = NULL;
	}
}

D3DXMATRIX Decal::GetMatrix() const
{
	

	
	return this->matrix;
}

D3DXMATRIX Decal::GetWorldMatrix()
{
	/*
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->position.x, this->position.y, this->position.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->size, this->size, this->size);

	D3DXMATRIX nworld = scaling*translate;
	this->world = nworld;
	*/
	return this->world;
}

void Decal::SetWorldMatrix( D3DXMATRIX mat )
{
	this->world = mat;
}
