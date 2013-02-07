#include "Billboard.h"


Billboard::Billboard() 
:	zPosition(D3DXVECTOR3(-1.0f, -1.0f, -1.0f)), 
	zSize(D3DXVECTOR2(-1.0f, -1.0f)), 
	zColor(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)),
	zTextureResource(NULL)
{

	
}
Billboard::Billboard(D3DXVECTOR3 position, D3DXVECTOR2 size, D3DXVECTOR4 color) 
:	zPosition(position), 
	zSize(size),
	zColor(color),
	zTextureResource(NULL)
{
	
}

Billboard::~Billboard()
{
	if(this->zTextureResource) GetResourceManager()->DeleteTextureResource(this->zTextureResource);
}


//GET
Vector3 Billboard::GetPosition() const
{
	return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z);
}
Vector2 Billboard::GetSize() const
{
	return Vector2(this->zSize.x, this->zSize.y);
}
Vector4 Billboard::GetColor() const
{
	return Vector4(this->zColor.x, this->zColor.y, this->zColor.z, this->zColor.w);
}

//SET
void Billboard::SetPosition(Vector3 position)
{
	this->zPosition = D3DXVECTOR3(position.x, position.y, position.z);
}

void Billboard::SetSize(Vector2 size)
{
	this->zSize = D3DXVECTOR2(size.x, size.y);
}

void Billboard::SetColor(Vector4 color)
{
	this->zColor = D3DXVECTOR4(color.x, color.y, color.z, color.w);
}
