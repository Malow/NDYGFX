#include "Texture.h"

Texture::Texture() : zSRV(NULL)
{
	
}
Texture::Texture(ID3D11ShaderResourceView* SRV) : zSRV(SRV)
{

}

Texture::~Texture()
{
	if(this->zSRV) this->zSRV->Release(); //this->zSRV = NULL; //**tillman
}

