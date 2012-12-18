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
			this->zTextures[filePath] = new Texture();
			this->zTextures[filePath]->SetSRVPointer(SRV);
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
			this->zTextures[filePath] = new Texture(SRV);
			//Return newly created texture.
			return this->zTextures[filePath];
		}
	}

	//Return texture if it already exists.
	return tex->second;
}


