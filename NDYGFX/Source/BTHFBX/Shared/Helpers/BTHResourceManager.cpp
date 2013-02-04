#include "BTHResourceManager.h"
#include "..\..\..\MaloWFileDebug.h"

#include "..\D3D\BTHTexture.h"
#include "..\..\..\Shader.h"

BTHResourceManager* BTHResourceManager::resourceManagerInstance = NULL;
BTHResourceManager* BTHResourceManager::GetInstance()
{
	if(!resourceManagerInstance)
	{
		resourceManagerInstance = new BTHResourceManager();
	}

	return resourceManagerInstance;
}

void BTHResourceManager::DeleteInstance()
{
	if(resourceManagerInstance)
	{
		delete resourceManagerInstance;
	}
}

BTHResourceManager::BTHResourceManager()
{

}

BTHResourceManager::~BTHResourceManager()
{
	Cleanup();
}

BTHTexture* BTHResourceManager::GetTexture(const char* filename, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	BTHTexture* texture = NULL;
	TEXTURE_MAP::iterator i  = mTextures.find(filename);

	if(i != mTextures.end() )
	{
		texture = i->second;
	}
	else
	{
		texture = LoadTexture(filename, dev, devCont);
		
		if(texture)
			mTextures[filename] = texture;
	}

	return texture;
}

BTHTexture* BTHResourceManager::LoadTexture(const char* filename, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	BTHTexture* texture = new BTHTexture();
	if(FAILED(texture->Init(filename, dev, devCont)))
	{
		SAFE_DELETE(texture);
	}

	return texture;
}

void BTHResourceManager::Cleanup()
{
	for(TEXTURE_MAP::iterator i = mTextures.begin(); i != mTextures.end(); i++)
	{
		SAFE_DELETE(i->second);
	}
}