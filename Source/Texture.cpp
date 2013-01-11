#include "Texture.h"

Texture::Texture() : zFilePath(""), zSRV(NULL)
{
	
}
Texture::Texture(ID3D11ShaderResourceView* SRV) : zFilePath(""), zSRV(SRV)
{

}
Texture::Texture(string filePath, ID3D11ShaderResourceView* SRV) : zFilePath(filePath), zSRV(SRV)
{

}

Texture::~Texture()
{
	if(this->zSRV) this->zSRV->Release(); this->zSRV = NULL;
}

