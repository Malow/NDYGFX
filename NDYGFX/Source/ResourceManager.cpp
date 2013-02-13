#include "ResourceManager.h"

//PRIVATE:
void ResourceManager::DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v)
{
	min.x = min(min.x, v.x);
	min.y = min(min.y, v.y);
	min.z = min(min.z, v.z);

	max.x = max(max.x, v.x);
	max.y = max(max.y, v.y);
	max.z = max(max.z, v.z);
}
MaloW::Array<MeshStrip*>* ResourceManager::LoadMeshStrips(const char* filePath, ObjData* objData, float& billboardHeight)
{
	MaloW::Array<MeshStrip*>* meshStrips = new MaloW::Array<MeshStrip*>();

	MaloW::Array<MaterialData>* mats = objData->mats;
	for(int q = 0; q < mats->size(); q++)
	{
		bool hasFace = false;
		MeshStrip* strip = new MeshStrip();


		/////// For hit/bounding boxes
		D3DXVECTOR3 min = D3DXVECTOR3(99999.9f, 99999.9f, 99999.9f);
		D3DXVECTOR3 max = -min;


		int nrOfVerts = 0;

		Vertex* tempverts = new Vertex[objData->faces->size()*3];

		for(int i = 0;  i < objData->faces->size(); i++)
		{
			if(objData->faces->get(i).material == mats->get(q).name)
			{
				int vertpos = objData->faces->get(i).data[0][0] - 1;
				int textcoord = objData->faces->get(i).data[0][1] - 1;
				int norm = objData->faces->get(i).data[0][2] - 1;
				tempverts[nrOfVerts] = Vertex(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
				DoMinMax(min, max, tempverts[nrOfVerts].pos);
				//For billboard
				if(billboardHeight < tempverts[nrOfVerts].pos.y)
				{
					billboardHeight = tempverts[nrOfVerts].pos.y;
				}
				nrOfVerts++;

				vertpos = objData->faces->get(i).data[2][0] - 1;
				textcoord = objData->faces->get(i).data[2][1] - 1;
				norm = objData->faces->get(i).data[2][2] - 1;
				tempverts[nrOfVerts] = Vertex(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
				DoMinMax(min, max, tempverts[nrOfVerts].pos);
				//For billboard
				if(billboardHeight < tempverts[nrOfVerts].pos.y)
				{
					billboardHeight = tempverts[nrOfVerts].pos.y;
				}
				nrOfVerts++;

				vertpos = objData->faces->get(i).data[1][0] - 1;
				textcoord = objData->faces->get(i).data[1][1] - 1;
				norm = objData->faces->get(i).data[1][2] - 1;
				tempverts[nrOfVerts] = Vertex(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
				DoMinMax(min, max, tempverts[nrOfVerts].pos);
				//For billboard
				if(billboardHeight < tempverts[nrOfVerts].pos.y)
				{
					billboardHeight = tempverts[nrOfVerts].pos.y;
				}
				nrOfVerts++;

				hasFace = true;
			}
		}

		if(!hasFace)
		{
			delete tempverts;
			delete strip;
		}
		else
		{
			strip->setNrOfVerts(nrOfVerts);
			Vertex* verts = new Vertex[nrOfVerts];
			for(int z = 0; z < nrOfVerts; z++)
			{
				verts[z] = tempverts[z];
			}
			delete tempverts;
			strip->SetVerts(verts);

			strip->SetTexturePath(objData->mats->get(q).texture);

			Material* mat = new Material();
			mat->AmbientColor = objData->mats->get(q).ka;
			if(mat->AmbientColor == D3DXVECTOR3(0.0f, 0.0f, 0.0f))				//////////// MaloW Fix, otherwise completely black with most objs
				mat->AmbientColor += D3DXVECTOR3(0.2f, 0.2f, 0.2f);			//////////// MaloW Fix, otherwise completely black with most objs

			mat->DiffuseColor = objData->mats->get(q).kd;
			if(mat->DiffuseColor == D3DXVECTOR3(0.0f, 0.0f, 0.0f))				//////////// MaloW Fix, otherwise completely black with most objs
				mat->DiffuseColor += D3DXVECTOR3(0.6f, 0.6f, 0.6f);			//////////// MaloW Fix, otherwise completely black with most objs

			mat->SpecularColor = objData->mats->get(q).ks;
			strip->SetMaterial(mat);

			strip->SetBoundingSphere(BoundingSphere(min, max));

			meshStrips->add(strip);
		}
	}

	return meshStrips;
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

			//Release mutex and return.
			ReleaseMutex(this->mutex);
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

	//Release mutex and return.
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

	//Release mutex.
	ReleaseMutex(this->mutex);
}










//PUBLIC:
ResourceManager::ResourceManager() : gDevice(NULL), gDeviceContext(NULL)
{
	this->mutex = CreateMutex(NULL, false, NULL);
}

ResourceManager::~ResourceManager()
{
	//Do not release device & device context, this is done by DxManager.

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



	//MeshStrip resources
	map<std::string, MeshStripsResource*>::iterator meshStripIterator;
	for(meshStripIterator = this->zMeshStripsResources.begin(); meshStripIterator != this->zMeshStripsResources.end(); meshStripIterator++)
	{
		if(meshStripIterator->second)
		{
			//A mesh strip resource cannot be deleted since it's destructor is private to force correct use of mesh strip resource creation/deletion.
			//Instead decrease reference count until it deletes itself.
			int refCount = meshStripIterator->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the mesh strip resource: '" 
				+ meshStripIterator->second->GetName() 
				+ "'; missing decrease(s) in reference counter somewhere. Occurrences: " 
				+ MaloW::convertNrToString(refCount - 1)
				+ ". Keep in mind that the cause can be PreLoadResources()-function if the resource was loaded but not used.");
			for(int i = 0; i < refCount; i++)
			{
				meshStripIterator->second->DecreaseReferenceCount();
			}

			meshStripIterator->second = NULL;
		}
	}
	this->zMeshStripsResources.clear();

	//Mesh/billboard heights
	this->zMeshHeights.clear();

	CloseHandle(this->mutex);
}


bool ResourceManager::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->gDevice = device;
	this->gDeviceContext = deviceContext;

	return true;
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
							MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + string(resourcesFileName) + "' was preloaded.");
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

									MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + string(resourcesFileName) + "' was preloaded.");
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

							MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + string(resourcesFileName) + "' was preloaded.");
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

TextureResource* ResourceManager::CreateTextureResourceFromFile( const char* filePath )
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateTextureResourceFromFile.");


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

			//Release mutex and return
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zTextureResources[filePath] = new TextureResource(filePath, SRV);
			//Increase reference count.
			this->zTextureResources[filePath]->IncreaseReferenceCount();
			
			//Release mutex and return
			ReleaseMutex(this->mutex);
			//Return newly created texture.
			return this->zTextureResources[filePath];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[filePath]->IncreaseReferenceCount();
	
	//Release mutex and return
	ReleaseMutex(this->mutex);
	return tex->second;
}
TextureResource* ResourceManager::CreateCubeTextureResourceFromFile( const char* filePath )
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateCubeTextureReosurceFromFile.");


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

			//Release mutex and return
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Create & Set texture if loading was successful.
			this->zTextureResources[filePath] = new TextureResource(filePath, SRV);
			//Increase reference count.
			this->zTextureResources[filePath]->IncreaseReferenceCount();

			//Release mutex and return
			ReleaseMutex(this->mutex);
			//Return newly created texture.
			return this->zTextureResources[filePath];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[filePath]->IncreaseReferenceCount();

	//Release mutex and return
	ReleaseMutex(this->mutex);
	return tex->second;
}

void ResourceManager::DeleteTextureResource( TextureResource* &textureResource )
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager DeleteTextureResource.");


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
				MaloW::Debug("	ResourceManager::DeleteTextureResource: Could not find the specified texture: '" + textureResource->GetName() + "', references: " + MaloW::convertNrToString(refCount));
			}
			textureResource = NULL;
		}
	}

	//Release mutex 
	ReleaseMutex(this->mutex);
}



BufferResource* ResourceManager::CreateBufferResource(const char* fileName, BUFFER_INIT_DESC bufferInitDesc)
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateBufferResource.");


	auto buff = this->zBufferResources.find(fileName);
	//If the buffer resource was not found in the array, create it.
	if(buff == this->zBufferResources.end())
	{
		//Create buffer
		Buffer* buffer = new Buffer();
		//Initialize buffer with device & deviceContext
		HRESULT hr = buffer->Init(this->gDevice, this->gDeviceContext, bufferInitDesc);
		if(FAILED(hr))
		{
			delete buffer; 
			buffer = NULL;

			MaloW::Debug("ERROR: ResourceManager: Could not create buffer: '" + string(fileName) + "'."
				+ string("ERROR code: '") 
				+ MaloW::GetHRESULTErrorCodeString(hr));

			//Release mutex and return.
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zBufferResources[fileName] = new BufferResource(fileName, buffer);
			//Increase reference count.
			this->zBufferResources[fileName]->IncreaseReferenceCount();

			//Release mutex and return.
			ReleaseMutex(this->mutex);
			//Return newly created buffer.
			return this->zBufferResources[fileName];
		}
	}

	//If the buffer already exists, increase reference counter & return it.
	this->zBufferResources[fileName]->IncreaseReferenceCount();

	//Release mutex and return.
	ReleaseMutex(this->mutex);
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
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager DeleteBufferResource.");


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


	//Release mutex and return.
	ReleaseMutex(this->mutex);
}


MeshStripsResource* ResourceManager::CreateMeshStripsResourceFromFile(const char* filePath, float& billboardHeight)
{

	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for ResourceManager: CreateMeshStripsResourceFromFile().");


	ObjectDataResource* tmp = this->LoadObjectDataResourceFromFile(filePath);

	MaloW::Debug("INFO: ResourceManager: CreateMeshStripsResourceFromFile(): the resource: '" + string(filePath) + "' was loaded.");
	
	if(tmp != NULL)
	{
		//Don't forget to decrease the reference pointer since its the resourcemanager loading the object. //**TILLMAN**
		tmp->DecreaseReferenceCount();
		
		ObjData* objData = tmp->GetObjectDataPointer();
		if(objData != NULL)
		{
			string file = string(filePath);
			
			file = file.substr(0, file.length() - 4);
			file += "Mesh";

			auto findResource = this->zMeshStripsResources.find(filePath);
			//Check if the meshStripResource already exists
			if(findResource == this->zMeshStripsResources.end())
			{
				//Create
				MaloW::Array<MeshStrip*>* meshStrips = NULL;
				//Load mesh strips
				meshStrips = this->LoadMeshStrips(filePath, objData, billboardHeight);
				//Create MeshStripsResource with the loaded mesh strips.
				this->zMeshStripsResources[filePath] = new MeshStripsResource(filePath, meshStrips);
				//Store billboard/mesh height
				this->zMeshHeights[filePath] = billboardHeight;
				//Increase reference counter, release mutex, and return.
				this->zMeshStripsResources[filePath]->IncreaseReferenceCount();
				ReleaseMutex(this->mutex);
				return this->zMeshStripsResources[filePath];
			}
			else
			{
				//If the MeshStripsResource already exists, increase reference counter & return it.
				findResource->second->IncreaseReferenceCount();
				//Set billboard/mesh height
				billboardHeight = this->zMeshHeights.find(filePath)->second;
				//Release mutex and return.
				ReleaseMutex(this->mutex);
				return findResource->second;
			}
		}
		else
		{
			MaloW::Debug("WARNING: ResourceManager: CreateMeshStripsResourceFromFile(): Could not create MeshStripsResource.");
		}
	}
	else
	{
		MaloW::Debug("WARNING: ResourceManager: CreateMeshStripsResourceFromFile(): Could not create MeshStripsResource.");
	}


	//Release mutex and return.
	ReleaseMutex(this->mutex);
	return NULL;
}
void ResourceManager::DeleteMeshStripsResource(MeshStripsResource* &meshStripsResource)
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for ResourceManager: DeleteMeshStripsResource().");


	if(meshStripsResource)
	{
		meshStripsResource->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(meshStripsResource->GetReferenceCount() == 1)
		{
			//Find resource.
			auto findResource = this->zMeshStripsResources.find(meshStripsResource->GetName());
			//If found..
			if(findResource != this->zMeshStripsResources.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				findResource->second->DecreaseReferenceCount();
				//Remove buffer resource from table.
				this->zMeshStripsResources.erase(findResource);
			}
			meshStripsResource = NULL;
		}
	}

	//Release mutex.
	ReleaseMutex(this->mutex);
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