#include "ResourceManager.h"
 

ResourceManager::ResourceManager() : gDevice(NULL), gDeviceContext(NULL)
{
	this->mutex = CreateMutex(NULL, false, NULL);
}

ResourceManager::~ResourceManager()
{
	//Do not release device & device context, this is done by DxManager.

	//Delete any remaining resources
	//Texture resources
	map<std::string, TextureResource*>::iterator texIterator;
	for(texIterator = this->zTextureResources.begin(); texIterator != this->zTextureResources.end(); texIterator++)
	{
		if(texIterator->second)
		{
			//A texture cannot be deleted since it's destructor is private to force correct use of texture creation/deletion.
			//Instead decrease reference count until it deletes itself.
			int refCount = texIterator->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the texture resource: " + texIterator->second->GetName() + "; missing decrease(s) in reference counter somewhere. Occurrences: " + MaloW::convertNrToString(refCount));
			for(int i = 0; i < refCount; i++)
			{
				texIterator->second->DecreaseReferenceCount();
			}

			texIterator->second = NULL;
		}
	}
	this->zTextureResources.clear();

	//Object data resources
	map<std::string, ObjectDataResource*>::iterator objDataIterator;
	for(objDataIterator = this->zObjectDataResources.begin(); objDataIterator != this->zObjectDataResources.end(); objDataIterator++)
	{
		if(objDataIterator->second)
		{
			//An object data cannot be deleted since it's destructor is private to force correct use of texture creation/deletion.
			//Instead decrease reference count until it deletes itself.
			int refCount = objDataIterator->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the object resource: " + objDataIterator->second->GetName() + "; missing decrease(s) in reference counter somewhere. Occurrences: " + MaloW::convertNrToString(refCount));
			for(int i = 0; i < refCount; i++)
			{
				objDataIterator->second->DecreaseReferenceCount();
			}

			objDataIterator->second = NULL;
		}
	}
	this->zObjectDataResources.clear();
	

	//Buffer resources
	map<std::string, BufferResource*>::iterator bufferIterator;
	for(bufferIterator = this->zBufferResources.begin(); bufferIterator != this->zBufferResources.end(); bufferIterator++)
	{
		if(bufferIterator->second)
		{
			//A buffer resource cannot be deleted since it's destructor is private to force correct use of buffer resource creation/deletion.
			//Instead decrease reference count until it deletes itself.
			int refCount = bufferIterator->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the buffer resource: " + bufferIterator->second->GetName() + "; missing decrease(s) in reference counter somewhere. Occurrences: " + MaloW::convertNrToString(refCount));
			for(int i = 0; i < refCount; i++)
			{
				bufferIterator->second->DecreaseReferenceCount();
			}

			bufferIterator->second = NULL;
		}
	}
	this->zBufferResources.clear();

	CloseHandle(this->mutex);
}


bool ResourceManager::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->gDevice = device;
	this->gDeviceContext = deviceContext;

	return true;
}

TextureResource* ResourceManager::CreateTextureResourceFromFile( const char* filePath )
{
	auto tex = this->zTextureResources.find(filePath);
	//If the texture resource was not found in the array, create it.
	if(tex == this->zTextureResources.end())
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
			//Create if loading was successful.
			this->zTextureResources[filePath] = new TextureResource(filePath, SRV);
			//Increase reference count.
			this->zTextureResources[filePath]->IncreaseReferenceCount();
			//Return newly created texture.
			return this->zTextureResources[filePath];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[filePath]->IncreaseReferenceCount();
	
	return tex->second;
}
TextureResource* ResourceManager::CreateCubeTextureResourceFromFile( const char* filePath )
{
	auto tex = this->zTextureResources.find(filePath);
	//If the texture was not found in the array, create it.
	if(tex == this->zTextureResources.end())
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
			//Create & Set texture if loading was successful.
			this->zTextureResources[filePath] = new TextureResource(filePath, SRV);
			//Increase reference count.
			this->zTextureResources[filePath]->IncreaseReferenceCount();
			//Return newly created texture.
			return this->zTextureResources[filePath];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[filePath]->IncreaseReferenceCount();

	return tex->second;
}

void ResourceManager::DeleteTextureResource( TextureResource* &textureResource )
{
	if(textureResource)
	{
		textureResource->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(textureResource->GetReferenceCount() == 1)
		{
			//Find texture resource.
			auto tex = this->zTextureResources.find(textureResource->GetName());
			//If found..
			if(tex != this->zTextureResources.end())
			{
				/*if(textureResource->GetName() == "Media/Bush_leaf_01_v07.png") //**TILLMAN MEM LEAK
				{
					float derp = 1.0f;

				}*/

				//Decrease reference counter once more so that the texture will delete itself.
				tex->second->DecreaseReferenceCount();
				//Remove texture from table.
				this->zTextureResources.erase(tex);
			}
			else
			{
				int refCount = textureResource->GetReferenceCount();
				for(int i = 0; i < refCount; i++)
				{
					//textureResource->DecreaseReferenceCount(); //**TILLMAN
				}
				MaloW::Debug("WARNING: DETECTED MEMORY LEAK!:");
				MaloW::Debug("ResourceManager::DeleteTextureResource: Could not find the specified texture:" + textureResource->GetName() + ", references: " + MaloW::convertNrToString(refCount));
			}
			textureResource = NULL;
		}
	}
}


ObjectDataResource* ResourceManager::LoadObjectDataResourceFromFile(const char* filePath)
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager Loadobjectdata.");



	auto objData = this->zObjectDataResources.find(filePath);
	//If the object data resource was not found in the array, create it.
	if(objData == this->zObjectDataResources.end())
	{
		//Create and load object data.
		ObjData* objectData = NULL;
		ObjLoader oj;
		objectData = oj.LoadObjFile(filePath);

		if(objectData == NULL)
		{
			string dbgStr = "WARNING: Failed to load object data from file: ";
			dbgStr += filePath;
			MaloW::Debug(dbgStr);

			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zObjectDataResources[filePath] = new ObjectDataResource(filePath, objectData);
			//Increase reference count.
			this->zObjectDataResources[filePath]->IncreaseReferenceCount();
			
			//Return newly created object data resource.
			return this->zObjectDataResources[filePath];
		}
	}

	//If the object data resource already exists, increase reference counter & return it.
	this->zObjectDataResources[filePath]->IncreaseReferenceCount();

	ReleaseMutex(this->mutex);
	return objData->second;
}

void ResourceManager::UnloadObjectDataResource(const char* filePath)
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager UnloadObjectData.");


	auto objData = this->zObjectDataResources.find(filePath);
	//If the object data resource was found in the array, decrease its reference counter.
	if(objData != this->zObjectDataResources.end())
	{
		ObjectDataResource* objectData = objData->second;
		objectData->DecreaseReferenceCount();

		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(objectData->GetReferenceCount() == 1)
		{
			//therefore delete it by decreasing its reference count one more time.
			objectData->DecreaseReferenceCount();
			//Remove object data resource from table.
			this->zObjectDataResources.erase(objData);
		}
	}
	else
	{
		MaloW::Debug("WARNING: ResourceManager::UnloadObjectDataResource(): Could not find the following object data resource to unload: " + string(filePath));
	}
	
	ReleaseMutex(this->mutex);
}

BufferResource* ResourceManager::CreateBufferResource(const char* fileName, BUFFER_INIT_DESC bufferInitDesc)
{
	auto buff = this->zBufferResources.find(fileName);
	//If the buffer resource was not found in the array, create it.
	if(buff == this->zBufferResources.end())
	{
		//Create buffer
		Buffer* buffer = new Buffer();
		//Initialize buffer with device & deviceContext
		if(FAILED(buffer->Init(this->gDevice, this->gDeviceContext, bufferInitDesc)))
		{
			MaloW::Debug("ERROR: ResourceManager: Could not create buffer:" + string(fileName));
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zBufferResources[fileName] = new BufferResource(fileName, buffer);
			//Increase reference count.
			this->zBufferResources[fileName]->IncreaseReferenceCount();
			//Return newly created buffer.
			return this->zBufferResources[fileName];
		}
	}

	//If the buffer already exists, increase reference counter & return it.
	this->zBufferResources[fileName]->IncreaseReferenceCount();

	return buff->second;

}
bool ResourceManager::HasBuffer(const char* fileName)
{
	//Find exact match. ***partial TILLMAN**
	auto buff = this->zBufferResources.find(fileName + string("Vertex")); //**TIllman, behöver bara kolla efter vertex buffer**
	//If the buffer resource was not found in the array, return false.
	if(buff == this->zBufferResources.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ResourceManager::DeleteBufferResource( BufferResource* &bufferResource )
{
	if(bufferResource)
	{
		bufferResource->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(bufferResource->GetReferenceCount() == 1)
		{
			//Find buffer resource.
			auto buff = this->zBufferResources.find(bufferResource->GetName());
			//If found..
			if(buff != this->zBufferResources.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				buff->second->DecreaseReferenceCount();
				//Remove buffer resource from table.
				this->zBufferResources.erase(buff);
			}
			bufferResource = NULL;
		}
	}
}

/* /TILLMAN
void ResourceManager::PreloadObjects(unsigned int nrOfObjects, char const* const* const objectFileNames)
{
	if(objectFileNames != NULL)
	{
		ObjData* tmpObjData = NULL;
		for(unsigned int i = 0; i < nrOfObjects; i++)
		{
			if(objectFileNames != NULL)
			{
				tmpObjData = this->LoadObjectDataFromFile(objectFileNames[i]);
				if(tmpObjData == NULL)
				{
					/*		CIRCULÄR INCLUDE - TILLMAN					
					Mesh* tmpMesh = NULL;
					string tmpFileName = string(objectFileNames[i]);
					
					if(tmpFileName.substr(tmpFileName.length() - 4) == ".obj") 
					{
						tmpMesh = new StaticMesh(D3DXVECTOR3());
					}
					else if(tmpFileName.substr(tmpFileName.length() - 4) == ".ani")
					{
						tmpMesh = new AnimatedMesh(D3DXVECTOR3());
					}

					//tmp = tmpMesh->GetObjData(); //***TILLMAN

					tmpMesh->LoadFromFile(objectFileNames[i]);
					

					//todo: load objdata - TILLMAN**
					string tmpFileName = string(objectFileNames[i]);

					/*if(tmpFileName.substr(tmpFileName.length() - 4) == ".obj") //detta på insidan av ny funktion?
					{
						//tmpObjData = GetGraphics()->LoadObjDataFromFile(objectFileNames[i]);
					}
					else if(tmpFileName.substr(tmpFileName.length() - 4) == ".ani")  //detta på insidan av ny funktion?
					{
						//tmpObjData = GetGraphics()->LoadObjDataFromFile(objectFileNames[i]);
					}
					//this->SetObjectData(objectFileNames[i], tmpObjData);
					//todo: load textures - TILLMAN**
					//todo: load buffer - TILLMAN**
				}
			}
			else
			{
				MaloW::Debug("WARNING: ResourceManager: PreloadObjects(): element to preload was empty.");
			}
		}
	}
	else
	{
		MaloW::Debug("WARNING: ResourceManager: PreloadObjects(): array of objects to preload was empty.");
	}





	/* buffer & texture
	string resourceNameVertices = mesh->GetFilePath() + string("Strip") + MaloW::convertNrToString(i) + string("Vertices");
	BufferResource* verts = GetResourceManager()->CreateBufferResource(resourceNameVertices.c_str(), bufferDesc);

	BufferResource* inds = NULL; 
	if(strip->getIndicies())
	{
		BUFFER_INIT_DESC bufferInds;
		bufferInds.ElementSize = sizeof(int);
		bufferInds.InitData = strip->getIndicies();
		bufferInds.NumElements = strip->getNrOfIndicies();
		bufferInds.Type = INDEX_BUFFER;
		bufferInds.Usage = BUFFER_DEFAULT;


		string resourceNameIndices = mesh->GetFilePath() + string("Strip") + MaloW::convertNrToString(i) + string("Indices");
		inds = GetResourceManager()->CreateBufferResource(resourceNameIndices.c_str(), bufferInds);
	}

	TextureResource* texture = NULL;
	if(strip->GetTexturePath() != "")
	{
		texture = GetResourceManager()->CreateTextureResourceFromFile(strip->GetTexturePath().c_str());
	}
}


*/






namespace
{
	static ResourceManager* resourceManager = NULL;
}

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	resourceManager = new ResourceManager();
	
	return resourceManager->Init(device, deviceContext);
}

ResourceManager* GetResourceManager() throw(...)
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