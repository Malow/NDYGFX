#include "Image.h"

Image::Image(D3DXVECTOR2 pos, D3DXVECTOR2 dim)
{
	this->position = pos;
	this->dimensions = dim;
	this->textureResource = NULL;
	this->opacity = 1.0f;
	this->strata = 0.5f;
}

Image::~Image()
{
	if(this->textureResource) GetResourceManager()->DeleteTextureResource(this->textureResource);
}

Vector2 Image::GetPosition() const
{
	return Vector2(this->position.x, this->position.y);
}

void Image::SetPosition( Vector2 pos )
{
	this->position = D3DXVECTOR2(pos.x, pos.y);
}

Vector2 Image::GetDimensions() const
{
	return Vector2(this->dimensions.x, this->dimensions.y);
}

void Image::SetDimensions( Vector2 dims )
{
	this->dimensions = D3DXVECTOR2(dims.x, dims.y);
}

void Image::SetStrata( float strata )
{
	this->strata = strata / 1000.0f;
}

float Image::GetStrata() const
{
	return this->strata * 1000.0f;
}
