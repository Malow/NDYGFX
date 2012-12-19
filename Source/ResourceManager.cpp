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
			delete it->second;
			it->second = NULL;
			counter++;
			string dbgstr = "WARNING: Resource manager deleted a resource; missing a decrease in reference counter somewhere. Occurrences: ";
			dbgstr += MaloW::convertNrToString(counter);
			MaloW::Debug(dbgstr);
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

ID3D11ShaderResourceView* ResourceManager::CreateShaderResourceViewFromFile( const char* filePath )
{
	auto tex = this->zTextures.find(filePath);
	//If the shader resource view was not found in the array, create it.
	if(tex == this->zTextures.end())
	{
		ID3D11ShaderResourceView* SRV = NULL;
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//Create 
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
			//Create & Set shader resource view pointer if not failed to load texture.
			this->zTextures[filePath] = new Texture(filePath, SRV);
			//Return newly created shader resource view.
			return this->zTextures[filePath]->GetSRVPointer();
		}
	}
	
	//Return shader resource view if already created/in the array.
	return tex->second->GetSRVPointer();
}


Texture* ResourceManager::CreateTextureFromFile( const char* filePath )
{
	auto tex = this->zTextures.find(filePath);
	//If the shader resource view was not found in the array, create it.
	if(tex == this->zTextures.end())
	{
		ID3D11ShaderResourceView* SRV = NULL;
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//Create 
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

	//It the texture already exists, increase reference counter & return texture.
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