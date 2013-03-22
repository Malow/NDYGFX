#include "BillboardCollection.h"


void BillboardCollection::RecalculateMinAndMaxPos()
{
	if(this->pVerticesChanged)
	{
		for(unsigned int i = 0; i < this->zNrOfVertices; ++i)
		{
			D3DXVec3Minimize(&this->zMinPos, &this->zMinPos, &this->zVertices[i].GetPosition());
			D3DXVec3Maximize(&this->zMaxPos, &this->zMaxPos, &this->zVertices[i].GetPosition());
		}
		this->pVerticesChanged = false;
	}
}




BillboardCollection::BillboardCollection() 
:	zVertices(NULL), zTextureResource(NULL), zOffsetVector(0.0f, 0.0f, 0.0f), zRenderShadowFlag(true), 
	zCullNearDistance(0.0f), zCullFarDistance(std::numeric_limits<float>::infinity()),
	zIsCameraCulled(false), zIsShadowCulled(false), zMinPos(0.0f, 0.0f, 0.0f), zMaxPos(0.0f, 0.0f, 0.0f), 
	pVerticesChanged(false)
{

	
}

BillboardCollection::BillboardCollection(unsigned int nrOfVertices, const VertexBillboard1* vertices, 
										 const D3DXVECTOR3& offsetVector, float cullNearDistance, float cullFarDistance)
:	zNrOfVertices(nrOfVertices), zTextureResource(NULL), zOffsetVector(offsetVector), 
	zCullNearDistance(cullNearDistance), zCullFarDistance(cullFarDistance),
	zIsCameraCulled(false), zIsShadowCulled(false), zMinPos(0.0f, 0.0f, 0.0f), zMaxPos(0.0f, 0.0f, 0.0f), 
	pVerticesChanged(true)
{
	this->zVertices = new VertexBillboard1[nrOfVertices];
	for(unsigned int i = 0; i < nrOfVertices; ++i)
	{
		this->zVertices[i] = vertices[i];
		this->zBillboardData.push_back(BillboardData(this->zVertices[i], NULL));
	}
}
BillboardCollection::BillboardCollection(unsigned int nrOfVertices, const D3DXVECTOR3* positions, 
										 const D3DXVECTOR2* sizes, const D3DXVECTOR3* colors, const D3DXVECTOR3& offsetVector,
										 float cullNearDistance, float cullFarDistance) 
:	zNrOfVertices(nrOfVertices), zTextureResource(NULL), zOffsetVector(offsetVector), 
	zCullNearDistance(cullNearDistance), zCullFarDistance(cullFarDistance),
	zIsCameraCulled(false), zIsShadowCulled(false), zMinPos(0.0f, 0.0f, 0.0f), zMaxPos(0.0f, 0.0f, 0.0f), 
	pVerticesChanged(true)
{
	
	this->zVertices = new VertexBillboard1[nrOfVertices];
	for(unsigned int i = 0; i < nrOfVertices; ++i)
	{
		this->zVertices[i] = VertexBillboard1(positions[i], sizes[i], colors[i]);
		this->zBillboardData.push_back(BillboardData(this->zVertices[i], NULL));
	}
}

BillboardCollection::~BillboardCollection()
{
	if(this->zVertices)
	{
		delete[] this->zVertices; 
		this->zVertices = NULL;
	}
	this->zBillboardData.clear();
	if(this->zTextureResource)
	{
		GetResourceManager()->DeleteTextureResource(this->zTextureResource);
	}
	
}
const D3DXVECTOR3& BillboardCollection::GetMinPos()
{ 
	this->RecalculateMinAndMaxPos();

	return this->zMinPos;
}
const D3DXVECTOR3& BillboardCollection::GetMaxPos()
{ 
	this->RecalculateMinAndMaxPos();

	return this->zMaxPos;
}

D3DXVECTOR3 BillboardCollection::CalcWorldPos()
{
	this->RecalculateMinAndMaxPos();

	return (this->zMinPos,this->zMaxPos) * 0.5f;
}

void BillboardCollection::SetTextureResource(TextureResource* textureResource)
{
	if(this->zTextureResource)
	{
		GetResourceManager()->DeleteTextureResource(this->zTextureResource);
	}
	
	this->zTextureResource = textureResource;
}
	
	
			
	
	
/*
//GET
Vector3 Billboard::GetPosition() const
{
	return Vector3(this->zVertex.pos.x, this->zVertex.pos.y, this->zVertex.pos.z);
}
Vector2 Billboard::GetTexCoord() const
{
	return Vector2(this->zVertex.texAndSize.x, this->zVertex.texAndSize.y);
}
Vector2 Billboard::GetSize() const
{
	return Vector2(this->zVertex.texAndSize.z, this->zVertex.texAndSize.w);
}
Vector3 Billboard::GetColor() const
{
	return Vector3(this->zVertex.color.x, this->zVertex.color.y, this->zVertex.color.z);
}


//SET
void Billboard::SetPosition(Vector3 position)
{
	this->zVertex.pos = D3DXVECTOR3(position.x, position.y, position.z);
}

void Billboard::SetTexCoord(Vector2 texCoord)
{
	this->zVertex.texAndSize = D3DXVECTOR4(texCoord.x, texCoord.y, this->zVertex.texAndSize.z, this->zVertex.texAndSize.w);
}

void Billboard::SetSize(Vector2 size)
{
	this->zVertex.texAndSize = D3DXVECTOR4(this->zVertex.texAndSize.x, this->zVertex.texAndSize.y, size.x, size.y);
}

void Billboard::SetColor(Vector3 color)
{
	this->zVertex.color = D3DXVECTOR3(color.x, color.y, color.z);
}
*/