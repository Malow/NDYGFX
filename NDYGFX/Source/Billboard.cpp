/*#include "Billboard.h"

Billboard::Billboard(D3DXVECTOR3 position, D3DXVECTOR2 size) : zPosition(position), zSize(size)
{
	
}

Billboard::~Billboard()
{
	if(this->zTextureResource) GetResourceManager()->DeleteTextureResource(this->zTextureResource);
}

Vector3 Billboard::GetPosition() const
{
	return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z);
}
Vector2 Billboard::GetSize() const
{
	return Vector2(this->zSize.x, this->zSize.y);
}

void Billboard::SetPosition(Vector3 position)
{
	this->zPosition = D3DXVECTOR3(position.x, position.y, position.z);
}

void Billboard::SetSize(Vector2 size)
{
	this->zSize = D3DXVECTOR2(size.x, size.y);
}
*/