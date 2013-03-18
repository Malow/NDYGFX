#include "Billboard.h"


Billboard::Billboard() 
:	zVertex(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f)), zTextureResource(NULL),
	zRenderShadowFlag(true), zIsCameraCulled(false), zIsShadowCulled(false)
{

	
}

Billboard::Billboard(const VertexBillboard1& vertex, bool renderShadowFlag)
:	zVertex(vertex), zTextureResource(NULL), zRenderShadowFlag(renderShadowFlag), 
	zIsCameraCulled(false), zIsShadowCulled(false)
{

}
Billboard::Billboard(const D3DXVECTOR3& position, const D3DXVECTOR2& size, const D3DXVECTOR3& color, bool renderShadowFlag) 
:	zVertex(position, size, color), zTextureResource(NULL), zRenderShadowFlag(renderShadowFlag),
	zIsCameraCulled(false), zIsShadowCulled(false)
{
	
}

Billboard::~Billboard()
{
	if(this->zTextureResource) GetResourceManager()->DeleteTextureResource(this->zTextureResource);
}



void Billboard::SetPosition(D3DXVECTOR3& position)
{
	this->zVertex.posAndSizeX.x = position.x;
	this->zVertex.posAndSizeX.y = position.y;
	this->zVertex.posAndSizeX.z = position.z;
}
void Billboard::SetSize(D3DXVECTOR2& size)
{
	this->zVertex.posAndSizeX.w = size.x;
	this->zVertex.sizeYAndColor.x = size.y;
}
void Billboard::SetColor(D3DXVECTOR3& color)
{
	this->zVertex.sizeYAndColor.y = color.x;
	this->zVertex.sizeYAndColor.z = color.y;
	this->zVertex.sizeYAndColor.w = color.z;
}






//** iBillboard interface functions **
//GET
/*
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
}*/
