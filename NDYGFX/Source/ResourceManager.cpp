#include "ResourceManager.h"
#include <algorithm>

void CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& normal)
{
	float length;

	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrtf((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}

void CalculateTangentBinormal(VertexNormalMap vertex1, VertexNormalMap vertex2, VertexNormalMap vertex3,
										  D3DXVECTOR3& tangent, D3DXVECTOR3& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.pos.x - vertex1.pos.x;
	vector1[1] = vertex2.pos.y - vertex1.pos.y;
	vector1[2] = vertex2.pos.z - vertex1.pos.z;

	vector2[0] = vertex3.pos.x - vertex1.pos.x;
	vector2[1] = vertex3.pos.y - vertex1.pos.y;
	vector2[2] = vertex3.pos.z - vertex1.pos.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.texCoord.x - vertex1.texCoord.x;
	tvVector[0] = vertex2.texCoord.y - vertex1.texCoord.y;

	tuVector[1] = vertex3.texCoord.x - vertex1.texCoord.x;
	tvVector[1] = vertex3.texCoord.y - vertex1.texCoord.y;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrtf((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrtf((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void CalculateModelVectors(VertexNormalMap* verts, int nrOfverts)
{
	int faceCount, i, index;
	VertexNormalMap vertex1, vertex2, vertex3;
	D3DXVECTOR3 tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = nrOfverts / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for(i=0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.pos.x = verts[index].pos.x;
		vertex1.pos.y = verts[index].pos.y;
		vertex1.pos.z = verts[index].pos.z;
		vertex1.texCoord.x = verts[index].texCoord.x;
		vertex1.texCoord.y = verts[index].texCoord.y;
		vertex1.normal.x = verts[index].normal.x;
		vertex1.normal.y = verts[index].normal.y;
		vertex1.normal.z = verts[index].normal.z;
		index++;

		vertex2.pos.x = verts[index].pos.x;
		vertex2.pos.y = verts[index].pos.y;
		vertex2.pos.z = verts[index].pos.z;
		vertex2.texCoord.x = verts[index].texCoord.x;
		vertex2.texCoord.y = verts[index].texCoord.y;
		vertex2.normal.x = verts[index].normal.x;
		vertex2.normal.y = verts[index].normal.y;
		vertex2.normal.z = verts[index].normal.z;
		index++;

		vertex3.pos.x = verts[index].pos.x;
		vertex3.pos.y = verts[index].pos.y;
		vertex3.pos.z = verts[index].pos.z;
		vertex3.texCoord.x = verts[index].texCoord.x;
		vertex3.texCoord.y = verts[index].texCoord.y;
		vertex3.normal.x = verts[index].normal.x;
		vertex3.normal.y = verts[index].normal.y;
		vertex3.normal.z = verts[index].normal.z;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structexCoord.xre.
		/*
		verts[index-1].normal.x = normal.x;
		verts[index-1].normal.y = normal.y;
		verts[index-1].normal.z = normal.z;*/
		verts[index-1].tangent.x = tangent.x;
		verts[index-1].tangent.y = tangent.y;
		verts[index-1].tangent.z = tangent.z;
		verts[index-1].binormal.x = binormal.x;
		verts[index-1].binormal.y = binormal.y;
		verts[index-1].binormal.z = binormal.z;
		/*
		verts[index-2].normal.x = normal.x;
		verts[index-2].normal.y = normal.y;
		verts[index-2].normal.z = normal.z;*/
		verts[index-2].tangent.x = tangent.x;
		verts[index-2].tangent.y = tangent.y;
		verts[index-2].tangent.z = tangent.z;
		verts[index-2].binormal.x = binormal.x;
		verts[index-2].binormal.y = binormal.y;
		verts[index-2].binormal.z = binormal.z;
		/*
		verts[index-3].normal.x = normal.x;
		verts[index-3].normal.y = normal.y;
		verts[index-3].normal.z = normal.z;*/
		verts[index-3].tangent.x = tangent.x;
		verts[index-3].tangent.y = tangent.y;
		verts[index-3].tangent.z = tangent.z;
		verts[index-3].binormal.x = binormal.x;
		verts[index-3].binormal.y = binormal.y;
		verts[index-3].binormal.z = binormal.z;
	}

	return;
}

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
	for(unsigned int q = 0; q < mats->size(); q++)
	{
		bool hasFace = false;
		MeshStrip* strip = new MeshStrip();


		/////// For hit/bounding boxes
		D3DXVECTOR3 min = D3DXVECTOR3(99999.9f, 99999.9f, 99999.9f);
		D3DXVECTOR3 max = -min;


		int nrOfVerts = 0;

		VertexNormalMap* tempverts = new VertexNormalMap[objData->faces->size()*3];
		bool faceMaterialMisMatch = false;
		int faceMaterialMisMatchIndex = -1;
		for(unsigned int i = 0;  i < objData->faces->size(); i++)
		{
			if(objData->faces->get(i).material == mats->get(q).name)
			{
				int vertpos = objData->faces->get(i).data[0][0] - 1;
				int textcoord = objData->faces->get(i).data[0][1] - 1;
				int norm = objData->faces->get(i).data[0][2] - 1;
				tempverts[nrOfVerts] = VertexNormalMap(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
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
				tempverts[nrOfVerts] = VertexNormalMap(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
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
				tempverts[nrOfVerts] = VertexNormalMap(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
				DoMinMax(min, max, tempverts[nrOfVerts].pos);
				//For billboard
				if(billboardHeight < tempverts[nrOfVerts].pos.y)
				{
					billboardHeight = tempverts[nrOfVerts].pos.y;
				}
				nrOfVerts++;

				hasFace = true;
			}
			else
			{
				faceMaterialMisMatch = true;
				faceMaterialMisMatchIndex = i;
			}
		}

		if(faceMaterialMisMatch)
		{
			string faceMaterial = objData->faces->get(faceMaterialMisMatchIndex).material;
			string material = mats->get(q).name;
			MaloW::Debug("WARNING: ResourceManager: LoadMeshStrips(): Material mismatch for object: '" + string(filePath) 
				+ ", Material # " + MaloW::convertNrToString(q) 
				+ "', Material: '" + material
				+ ", Face material: '" + faceMaterial + "'.");
		}

		if(!hasFace)
		{
			delete tempverts;
			delete strip;
		}
		else
		{
			strip->setNrOfVerts(nrOfVerts);
			VertexNormalMap* verts = new VertexNormalMap[nrOfVerts];
			for(int z = 0; z < nrOfVerts; z++)
			{
				verts[z] = tempverts[z];
			}
			delete tempverts;
			CalculateModelVectors(verts, nrOfVerts);
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


	string file = filePath;
	//Force lower case
	std::transform(file.begin(), file.end(), file.begin(), tolower); 
	auto objData = this->zObjectDataResources.find(file);
	//If the object data resource was not found in the array, create it.
	if(objData == this->zObjectDataResources.end())
	{
		//Create and load object data.
		ObjData* objectData = NULL;
		ObjLoader oj;
		objectData = oj.LoadObjFile(file);

		if(objectData == NULL)
		{
			string dbgStr = "WARNING: Failed to load object data from file: ";
			dbgStr += file;
			MaloW::Debug(dbgStr);

			//Release mutex and return.
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zObjectDataResources[file] = new ObjectDataResource(file, objectData);
			//Increase reference count.
			this->zObjectDataResources[file]->IncreaseReferenceCount();

			//Release mutex and return resource.
			ReleaseMutex(this->mutex);
			//Return newly created object data resource.
			return this->zObjectDataResources[file];
		}
	}

	//If the object data resource already exists, increase reference counter & return it.
	this->zObjectDataResources[file]->IncreaseReferenceCount();

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

	string file = filePath;
	//Force lower case
	std::transform(file.begin(), file.end(), file.begin(), tolower); 

	auto objData = this->zObjectDataResources.find(file);
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
ResourceManager::ResourceManager() : gDevice(NULL), gDeviceContext(NULL), zDeferredContext(NULL)
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
	HRESULT hr = this->gDevice->CreateDeferredContext(0, &this->zDeferredContext);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: Failed to create deferred context, Error code: '" + MaloW::convertNrToString((int)hr) + "'. Error msg: " + MaloW::GetHRESULTErrorCodeString(hr));
		return false;
	}

	return true;
}




void ResourceManager::PreLoadResources(unsigned int nrOfResources, std::vector<string> resourcesFileNames)
{
	//if(resourcesFileNames != NULL)
	{
		for(unsigned int i = 0; i < nrOfResources; i++)
		{
			const char* resourcesFileName = resourcesFileNames.at(i).c_str();
			if(resourcesFileName != NULL)
			{
				string tmpFileName = string(resourcesFileName); 
				//Force lower case
				std::transform(tmpFileName.begin(), tmpFileName.end(), tmpFileName.begin(), tolower); 
				
				if(tmpFileName.substr(tmpFileName.length() - 4) == ".obj") 
				{
					//First check if the resource has already been loaded.
					auto objData = this->zObjectDataResources.find(tmpFileName);
					if(objData == this->zObjectDataResources.end())
					{
						ObjectDataResource* objDataResPtr = this->LoadObjectDataResourceFromFile(tmpFileName.c_str());
						if(objDataResPtr != NULL)
						{
							//Decrease reference count since we're Preloading and no object yet has a reference to the resource.
							this->zObjectDataResources[tmpFileName]->DecreaseReferenceCount();
							MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + tmpFileName + "' was preloaded.");
						}
						//else write debug info, this is done by LoadObjectDataResourceFromFile(..).
					}
					else
					{
						MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + tmpFileName + "' has already been loaded.");
					}
				}
				else if(tmpFileName.substr(tmpFileName.length() - 4) == ".ani")
				{
					//Save the path to the folder the resource is in.
					string tempFilename = tmpFileName;
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
					inputFileAni.open(tmpFileName);
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
							//Force lower case
							std::transform(tmpResourceFileName.begin(), tmpResourceFileName.end(), tmpResourceFileName.begin(), tolower); 

							//First check if the resource has already been loaded.
							auto objData = this->zObjectDataResources.find(tmpResourceFileName);
							if(objData == this->zObjectDataResources.end())
							{
								ObjectDataResource* objDataResPtr = this->LoadObjectDataResourceFromFile(tmpResourceFileName.c_str());
								if(objDataResPtr != NULL)
								{
									//Decrease reference count since we're Preloading and no object yet has a reference to the resource.
									this->zObjectDataResources[tmpResourceFileName.c_str()]->DecreaseReferenceCount();

									MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + tmpFileName + "' was preloaded.");
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
					auto tex = this->zTextureResources.find(tmpFileName);
					if(tex == this->zTextureResources.end())
					{
						TextureResource* texResPtr = this->CreateTextureResourceFromFile(tmpFileName.c_str(), true);
						if(texResPtr != NULL)
						{
							//Decrease reference count since we're Preloading and no object yet has a reference to the resource.
							this->zTextureResources[tmpFileName]->DecreaseReferenceCount();

							MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + tmpFileName + "' was preloaded.");
						}
						//else write debug info, this is done by LoadObjectDataResourceFromFile(..).
					}
					else
					{
						MaloW::Debug("INFO: ResourceManager: PreLoadResources(): the resource: '" + tmpFileName + "' has already been loaded.");
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

TextureResource* ResourceManager::CreateTextureResourceFromFile( const char* filePath, bool generateMipMap)
{
	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else 
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateTextureResourceFromFile.");

	string file = filePath;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto tex = this->zTextureResources.find(file);
	//If the texture resource was not found in the array, create it.
	if(tex == this->zTextureResources.end())
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		loadInfo.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if(generateMipMap)
		{
			//loadInfo.MiscFlags =  D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		
		//Create 
		ID3D11ShaderResourceView* SRV = NULL;
		if(FAILED(D3DX11CreateShaderResourceViewFromFile(	
			this->gDevice, 
			file.c_str(),
			&loadInfo, 
			NULL, 
			&SRV,
			NULL)))
		{
			string dbgStr = "WARNING: Failed to load texture: ";
			dbgStr += file;
			MaloW::Debug(dbgStr);

			//Release mutex and return
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Generate mip maps.
			if(generateMipMap && this->zDeferredContext != NULL)
			{
				//this->zDeferredContext->GenerateMips(SRV);
			}

			//Create if loading was successful.
			this->zTextureResources[file] = new TextureResource(file, SRV);
			//Increase reference count.
			this->zTextureResources[file]->IncreaseReferenceCount();
			
			//Release mutex and return
			ReleaseMutex(this->mutex);
			//Return newly created texture resource.
			return this->zTextureResources[file];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[file]->IncreaseReferenceCount();
	
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


	string file = filePath;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto tex = this->zTextureResources.find(file);
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
			file.c_str(),
			&loadInfo, 
			NULL, 
			&SRV,
			NULL)))
		{
			string dbgStr = "WARNING: Failed to load cube texture: ";
			dbgStr += file;
			MaloW::Debug(dbgStr);

			//Release mutex and return
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Create & Set texture if loading was successful.
			this->zTextureResources[file] = new TextureResource(file, SRV);
			//Increase reference count.
			this->zTextureResources[file]->IncreaseReferenceCount();

			//Release mutex and return
			ReleaseMutex(this->mutex);
			//Return newly created texture.
			return this->zTextureResources[file];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[file]->IncreaseReferenceCount();

	//Release mutex and return
	ReleaseMutex(this->mutex);
	return tex->second;
}
const TextureResource* ResourceManager::GetTextureResourcePointer(const char* id) const
{
	string file = id;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto tex = this->zTextureResources.find(file);

	if(tex != this->zTextureResources.cend())
	{
		return tex->second;
	}

	return NULL;
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
					//textureResource->DecreaseReferenceCount(); 
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

	string file = fileName;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto buff = this->zBufferResources.find(file);
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

			MaloW::Debug("ERROR: ResourceManager: Could not create buffer: '" + file + "'."
				+ string("ERROR code: '") 
				+ MaloW::GetHRESULTErrorCodeString(hr));

			//Release mutex and return.
			ReleaseMutex(this->mutex);
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zBufferResources[file] = new BufferResource(file, buffer);
			//Increase reference count.
			this->zBufferResources[file]->IncreaseReferenceCount();

			//Release mutex and return.
			ReleaseMutex(this->mutex);
			//Return newly created buffer.
			return this->zBufferResources[file];
		}
	}

	//If the buffer already exists, increase reference counter & return it.
	this->zBufferResources[file]->IncreaseReferenceCount();

	//Release mutex and return.
	ReleaseMutex(this->mutex);
	return buff->second;
}
bool ResourceManager::HasBuffer(const char* fileName)
{
	string file = fileName;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	//Find exact match. 
	auto buff = this->zBufferResources.find(file + string("vertex")); 
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
	string file = filePath;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	if(this->mutex)
	{
		WaitForSingleObject(this->mutex, INFINITE);
	}
	else
	{
		MaloW::Debug("Mutex is broken / hasn't been created / has been closed for ResourceManager: CreateMeshStripsResourceFromFile().");
	}

	ObjectDataResource* tmp = this->LoadObjectDataResourceFromFile(file.c_str());

	if(tmp != NULL)
	{
		ObjData* objData = tmp->GetObjectDataPointer();
		if(objData != NULL)
		{
			//string file = string(filePath);
			
			file = file.substr(0, file.length() - 4);
			file += "_mesh";

			auto findResource = this->zMeshStripsResources.find(file);
			//Check if the meshStripResource already exists
			if(findResource == this->zMeshStripsResources.end())
			{
				//Create
				MaloW::Array<MeshStrip*>* meshStrips = NULL;
				//Load mesh strips
				meshStrips = this->LoadMeshStrips(file.c_str(), objData, billboardHeight);
				//Create MeshStripsResource with the loaded mesh strips.
				this->zMeshStripsResources[file] = new MeshStripsResource(file, meshStrips);
				//Store billboard/mesh height
				this->zMeshHeights[file] = billboardHeight;
				//Increase reference counter, release mutex, and return.
				this->zMeshStripsResources[file]->IncreaseReferenceCount();
				ReleaseMutex(this->mutex);
				return this->zMeshStripsResources[file];
			}
			else
			{
				//If the MeshStripsResource already exists, increase reference counter & return it.
				findResource->second->IncreaseReferenceCount();
				//Set billboard/mesh height
				billboardHeight = this->zMeshHeights.find(file)->second;
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
bool ResourceManager::HasMeshStripsResource(const char* fileName)
{
	string file = string(fileName);
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force to lower case

	string fileType = file.substr(file.length() - 3);
	if(strcmp(fileType.c_str(), "ani") == 0)
	{
		//Save the path to the folder the resource is in.
		//string tempFilename = resourcesFileName;
		//Save the path to the folder the resource is in.
		string tempFilename = file;
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
		inputFileAni.open(file);
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
				std::transform(path.begin(), path.end(), path.begin(), tolower); //Force to lower case

				string tmpResourceFileName = pathfolder + path;

				path = path.substr(0, path.length() - 4);
				path = pathfolder + path + "_mesh";

				auto find = this->zMeshStripsResources.find(path);
				if(find == this->zMeshStripsResources.cend())
				{
					return false;
				}
			}

			//Everything is loaded, return true
			return true;
		}
	}
	else if(strcmp(fileType.c_str(), "obj") == 0)
	{
		file = file.substr(0, file.length() - 4);
		file += "_mesh";

		auto find = this->zMeshStripsResources.find(file);
		if(find != this->zMeshStripsResources.cend())
		{
			return true;
		}
	}
	else
	{
		MaloW::Debug("WARNING: ResourceManager::HasMeshStripsResource(): Filetype not recognized: '" + fileType + "'. Full filename: '" + file + "'.");
		return false;
	}
	
	return false;
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
		//Decrease the reference counter for the data used to create the mesh strip resource
		string file = "";
		file = meshStripsResource->GetName().substr(0, meshStripsResource->GetName().length() - 5);
		file += ".obj";

		this->UnloadObjectDataResource(file.c_str());

		//Decrease the reference counter for the mesh strip resource.
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