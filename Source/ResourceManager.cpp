#include "ResourceManager.h"
 
ResourceManager::ResourceManager() : gDevice(NULL), gDeviceContext(NULL)
{
	
}

ResourceManager::~ResourceManager()
{
	//Do not release device & device context, this is done by DxManager.

	//Delete any remaining resources
	int counter = 0;
	map<std::string, Texture*>::iterator it;
	for(it = this->zTextures.begin(); it != this->zTextures.end(); it++)
	{
		//Delete any remaining textures
		if(it->second)
		{
			//A texture cannot be deleted since it's destructor is private to force correct use of textures
			//Instead decrease reference count until it deletes itself.
			int refCount = it->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the resource: " + it->second->GetName() + "; missing decrease(s) in reference counter somewhere. Occurrences: " + MaloW::convertNrToString(refCount));
			for(int i = 0; i < refCount; i++)
			{
				it->second->DecreaseReferenceCount();
			}

			it->second = NULL;
		}
	}
}


bool ResourceManager::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->gDevice = device;
	this->gDeviceContext = deviceContext;

	return true;
}

/*
ID3D11Texture2D* ResourceManager::CreateTexture2D(const char* filePath)
{


}*/

Texture* ResourceManager::CreateTextureFromFile( const char* filePath )
{
	auto tex = this->zTextures.find(filePath);
	//If the shader resource view was not found(already created) in the array, create it.
	if(tex == this->zTextures.end())
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//Create 
		ID3D11ShaderResourceView* SRV = NULL;
		if(FAILED(D3DX11CreateShaderResourceViewFromFile(	
			this->gDevice, 
			filePath,
			&loadInfo, 
			NULL, 
			&SRV,
			NULL)))
		{
			string dbgStr = "WARNING: Failed to load texture: ";
			dbgStr += filePath;
			MaloW::Debug(dbgStr);
			return NULL;
		}
		else
		{
			//Create & Set shader resource view pointer if loading was successful.
			this->zTextures[filePath] = new Texture(filePath, SRV);
			//Increase reference count.
			this->zTextures[filePath]->IncreaseReferenceCount();
			//Return newly created texture.
			return this->zTextures[filePath];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextures[filePath]->IncreaseReferenceCount();
	
	return tex->second;
}
Texture* ResourceManager::CreateCubeTexture( const char* filePath )
{
	auto tex = this->zTextures.find(filePath);
	//If the shader resource view was not found(already created) in the array, create it.
	if(tex == this->zTextures.end())
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		//Create 
		ID3D11ShaderResourceView* SRV = NULL;
		if(FAILED(D3DX11CreateShaderResourceViewFromFile(	
			this->gDevice, 
			filePath,
			&loadInfo, 
			NULL, 
			&SRV,
			NULL)))
		{
			string dbgStr = "WARNING: Failed to load cube texture: ";
			dbgStr += filePath;
			MaloW::Debug(dbgStr);
			return NULL;
		}
		else
		{
			//Create & Set shader resource view pointer if loading was successful.
			this->zTextures[filePath] = new Texture(filePath, SRV);
			//Increase reference count.
			this->zTextures[filePath]->IncreaseReferenceCount();
			//Return newly created texture.
			return this->zTextures[filePath];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextures[filePath]->IncreaseReferenceCount();

	return tex->second;
}

void ResourceManager::DeleteTexture( Texture* &texture )
{
	if(texture)
	{
		texture->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(texture->GetReferenceCount() == 1)
		{
			//Find texture.
			auto tex = this->zTextures.find(texture->GetName());
			//If found..
			if(tex != this->zTextures.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				tex->second->DecreaseReferenceCount();
				//Remove texture from table.
				this->zTextures.erase(tex);
			}
			texture = NULL;
		}
	}
}











namespace
{
	static ResourceManager* resourceManager = NULL;
}

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	resourceManager = new ResourceManager();
	
	return resourceManager->Init(device, deviceContext);
}

ResourceManager* GetResourceManager() throw(const char*)
{
	if(!resourceManager)
	{
		throw("ResourceManager Not Initialized!");
	}

	return resourceManager;
}

bool FreeResourceManager()
{
	if(resourceManager)
	{
		delete resourceManager;
		resourceManager = NULL;
	}
	else
	{
		return false;
	}	

	return true;
}