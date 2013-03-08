#include "Billboard.h"


Billboard::Billboard() 
:	/*zPosition(D3DXVECTOR3(-1.0f, -1.0f, -1.0f)), 
	zSize(D3DXVECTOR2(-1.0f, -1.0f)), 
	zColor(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)),
	*/
	zVertex(), zTextureResource(NULL)
{

	
}

Billboard::Billboard(const VertexBillboardCompressed1& vertex)
	: zVertex(vertex), zTextureResource(NULL)
{

}
Billboard::Billboard(const D3DXVECTOR3& position, const D3DXVECTOR2& texCoord, const D3DXVECTOR2& size, const D3DXVECTOR3& color) 
:	/*zPosition(position), 
	zSize(size),
	zColor(color),
	*/

	zVertex(position, texCoord, size, color), zTextureResource(NULL)
{
	
}

Billboard::~Billboard()
{
	if(this->zTextureResource) GetResourceManager()->DeleteTextureResource(this->zTextureResource);
}


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
