#include "TextureResource.h"

TextureResource::TextureResource() : zFilePath(""), zSRV(NULL)
{
	
}
TextureResource::TextureResource(ID3D11ShaderResourceView* SRV) : zFilePath(""), zSRV(SRV)
{

}
TextureResource::TextureResource(string filePath, ID3D11ShaderResourceView* SRV) : zFilePath(filePath), zSRV(SRV)
{

}

TextureResource::~TextureResource()
{
	if(this->zSRV) this->zSRV->Release(); this->zSRV = NULL;
}

