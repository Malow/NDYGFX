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
			MaloW::Debug("WARNING: Resource manager deleted the texture resource: '" + texIterator->second->GetName() 
				+ "'; missing decrease(s) in reference counter somewhere. Occurrences: " 
				+ MaloW::convertNrToString(refCount - 1) 
				+ ". Keep in mind that the cause can be PreLoadResources()-function if the resource was loaded but not used.");
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
			MaloW::Debug("WARNING: Resource manager deleted the object resource: '" 
				+ objDataIterator->second->GetName() 
				+ "'; missing decrease(s) in reference counter somewhere. Occurrences: " 
				+ MaloW::convertNrToString(refCount - 1)
				+ ". Keep in mind that the cause can be PreLoadResources()-function if the resource was loaded but not used.");
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
			MaloW::Debug("WARNING: Resource manager deleted the buffer resource: '" 
				+ bufferIterator->second->GetName() 
				+ "'; missing decrease(s) in reference counter somewhere. Occurrences: " 
				+ MaloW::convertNrToString(refCount - 1)
				+ ". Keep in mind that the cause can be PreLoadResources()-function if the resource was loaded but not used.");
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
				MaloW::Debug("ResourceManager::DeleteTextureResource: Could not find the specified texture: '" + textureResource->GetName() + "', references: " + MaloW::convertNrToString(refCount));
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
		objectData = oj.LoadObjFile(string(filePath));

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
			
			//Release mutex and return resource.
			ReleaseMutex(this->mutex);
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
		MaloW::Debug("WARNING: ResourceManager::UnloadObjectDataResource(): Could not find the following object data resource to unload: '" + string(filePath) + "'.");
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

void ResourceManager::PreLoadResources(unsigned int nrOfResources, char const* const* const resourcesFileNames)
{
	if(resourcesFileNames != NULL)
	{
		for(unsigned int i = 0; i < nrOfResources; i++)
		{
			const char* resourcesFileName = resourcesFileNames[i];
			if(resourcesFileName != NULL)
			{
				string tmpFileName = string(resourcesFileName); 
				if(tmpFileName.substr(tmpFileName.length() - 4) == ".obj") 
				{
					//First check if the resource has already been loaded.
					auto objData = this->zObjectDataResources.find(resourcesFileName);
					if(objData == this->zObjectDataResources.end())
					{
						ObjectDataResource* objDataResPtr = this->LoadObjectDataResourceFromFile(resourcesFileName);
						if(objDataResPtr != NULL)
						{
							//Decrease reference count since we're Preloading and no object yet has a reference to the resource.
							this->zObjectDataResources[resourcesFileName]->DecreaseReferenceCount();
						}
						//else write debug info, this is done by LoadObjectDataResourceFromFile(..).
					}
					else
					{
						MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + string(resourcesFileName) + "' has already been loaded.");
					}
				}
				else if(tmpFileName.substr(tmpFileName.length() - 4) == ".ani")
				{
					//Save the path to the folder the resource is in.
					string tempFilename = resourcesFileName;
					string pathfolder = "";
					size_t slashpos = tempFilename.find("/");
					while(slashpos != string::npos)
					{
						slashpos = tempFilename.find("/");
						pathfolder += tempFilename.substr(0, slashpos + 1);
						tempFilename = tempFilename.substr(slashpos + 1);
					}

					//Open the .ani-file.
					ifstream inputFileAni;
					inputFileAni.open(resourcesFileName);
					if(inputFileAni)
					{
						string line = "";
						getline(inputFileAni, line);
						
						int nrOfObjects = atoi(line.c_str());

						for(int a = 0; a < nrOfObjects; a++)
						{
							string path = "";
							//We're only interested in the object file name (.obj), so skip key frame time
							getline(inputFileAni, line);
							//Extract object file name.
							getline(inputFileAni, path);

							string tmpResourceFileName = pathfolder + path;

							//First check if the resource has already been loaded.
							auto objData = this->zObjectDataResources.find(tmpResourceFileName);
							if(objData == this->zObjectDataResources.end())
							{
								ObjectDataResource* objDataResPtr = this->LoadObjectDataResourceFromFile(tmpResourceFileName.c_str());
								if(objDataResPtr != NULL)
								{
									//Decrease reference count since we're Preloading and no object yet has a reference to the resource.
									this->zObjectDataResources[tmpResourceFileName.c_str()]->DecreaseReferenceCount();
								}
								//else write debug info, this is done by LoadObjectDataResourceFromFile(..).
							}
							else
							{
								MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + string(tmpResourceFileName) + "' has already been loaded.");
							}
						}
					}
				}
				else if (tmpFileName.substr(tmpFileName.length() - 4) == ".png" || tmpFileName.substr(tmpFileName.length() - 4) == ".dds")
				{
					//First check if the resource has already been loaded.
					auto tex = this->zTextureResources.find(resourcesFileName);
					if(tex == this->zTextureResources.end())
					{
						TextureResource* texResPtr = this->CreateTextureResourceFromFile(resourcesFileName);
						if(texResPtr != NULL)
						{
							//Decrease reference count since we're Preloading and no object yet has a reference to the resource.
							this->zTextureResources[resourcesFileName]->DecreaseReferenceCount();
						}
						//else write debug info, this is done by LoadObjectDataResourceFromFile(..).
					}
					else
					{
						MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + string(resourcesFileName) + "' has already been loaded.");
					}
				}
				else
				{
					MaloW::Debug("WARNING: ResourceManager: PreLoadResources(): the resource file format: '" + tmpFileName.substr(tmpFileName.length() - 4) + "' was not recognized/supported. (Filename: '" + tmpFileName + "'.");
				}
			}
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