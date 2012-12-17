#include "Texture.h"

Texture::Texture() : zSRV(NULL)
{
	
}

Texture::~Texture()
{
	if(this->zSRV) this->zSRV->Release(); 
}

