#include "BTHResourceManager.h"
#include "..\..\..\MaloWFileDebug.h"
#include "..\D3D\BTHTexture.h"
#include "..\..\..\Shader.h"


BTHResourceManager* BTHResourceManager::GetInstance()
{
	static BTHResourceManager manager;
	return &manager;
}

BTHResourceManager::BTHResourceManager()
{

}

BTHResourceManager::~BTHResourceManager()
{
	Cleanup();
}

BTHTexture* BTHResourceManager::GetTexture(const std::string& filename, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	BTHTexture* texture = NULL;

	zMutex.lock();
	auto i  = mTextures.find(filename);

	if(i != mTextures.end() )
	{
		zRefCounters[texture]++;
		texture = i->second;
	}
	else
	{
		texture = LoadTexture(filename, dev, devCont);
		
		if(texture)
		{
			zRefCounters[texture] = 1;
			mTextures[filename] = texture;
		}
	}
	zMutex.unlock();

	return texture;
}

void BTHResourceManager::FreeTexture( BTHTexture*& texture )
{
	zMutex.lock();
	auto i = zRefCounters.find(texture);
	if ( i != zRefCounters.end() ) 
	{
		i->second--;
		if ( i->second == 0 )
		{
			auto i2 = mTextures.find(texture->GetFilename());
			if ( i2 != mTextures.end() ) mTextures.erase(i2);

			zRefCounters.erase(i);

			delete texture;
		}
	}
	zMutex.unlock();
	
	texture = 0;
}

BTHTexture* BTHResourceManager::LoadTexture(const std::string& filename, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	BTHTexture* texture = new BTHTexture();
	if(FAILED(texture->Init(filename.c_str(), dev, devCont)))
	{
		SAFE_DELETE(texture);
	}

	return texture;
}

void BTHResourceManager::Cleanup()
{
	zMutex.lock();
	for(auto i = mTextures.begin(); i != mTextures.end(); i++)
	{
		SAFE_DELETE(i->second);
	}
	zMutex.unlock();
}