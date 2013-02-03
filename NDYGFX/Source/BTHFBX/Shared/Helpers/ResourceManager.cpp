#include "ResourceManager.h"

#include "../D3D/Device3D.h"
#include "../D3D/Texture.h"
#include "../D3D/Shader.h"

ResourceManager* ResourceManager::resourceManagerInstance = NULL;
ResourceManager* ResourceManager::GetInstance(Device3D* device3D)
{
	if(!resourceManagerInstance)
	{
		if(!device3D)
			MessageBox(0, "Need to provide Device3D* @ first call", "Texture Manager error!", MB_ICONERROR | MB_OK);
		else
			resourceManagerInstance = myNew ResourceManager(device3D);
	}

	return resourceManagerInstance;
}

void ResourceManager::DeleteInstance()
{
	if(resourceManagerInstance)
	{
		SAFE_DELETE(resourceManagerInstance);
	}
}

ResourceManager::ResourceManager(Device3D* device3D) : mDevice3D(device3D)
{

}

ResourceManager::~ResourceManager()
{
	Cleanup();
}

Texture* ResourceManager::GetTexture(const char* filename)
{
	Texture* texture = NULL;
	TEXTURE_MAP::iterator i  = mTextures.find(filename);

	if(i != mTextures.end() )
	{
		texture = i->second;
	}
	else
	{
		texture = LoadTexture(filename);
		
		if(texture)
			mTextures[filename] = texture;
	}

	return texture;
}

Texture* ResourceManager::LoadTexture(const char* filename)
{
	Texture* texture = myNew Texture();
	if(FAILED(texture->Init(mDevice3D, filename)))
	{
		SAFE_DELETE(texture);
	}

	return texture;
}

Shader* ResourceManager::GetShader(const char* filename,
	const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements)
{
	Shader* shader = NULL;
	SHADER_MAP::iterator i  = mShaders.find(filename);

	if(i != mShaders.end() )
	{
		shader = i->second;
	}
	else
	{
		shader = LoadShader(filename, inputElementDesc, numElements);
		
		if(shader)
			mShaders[filename] = shader;
	}

	return shader;
}

Shader* ResourceManager::LoadShader(const char* filename,
	const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements)
{
	Shader* shader = myNew Shader();
	if(FAILED(shader->Init(mDevice3D, filename, inputElementDesc, numElements)))
	{
		SAFE_DELETE(shader);
	}

	return shader;
}

void ResourceManager::Cleanup()
{
	for(TEXTURE_MAP::iterator i = mTextures.begin(); i != mTextures.end(); i++)
	{
		SAFE_DELETE(i->second);
	}

	for(SHADER_MAP::iterator i = mShaders.begin(); i != mShaders.end(); i++)
	{
		SAFE_DELETE(i->second);
	}
}