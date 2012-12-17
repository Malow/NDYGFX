#include "Texture.h"

Texture::Texture()
{
	this->zSRV = NULL;
}

Texture::~Texture()
{
	if(this->zSRV) this->zSRV->Release(); 
}

