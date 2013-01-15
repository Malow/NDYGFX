#include "ResourceManager.h"
 
/*
void ResourceManager::DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v)
{
	min.x = min(min.x, v.x);
	min.y = min(min.y, v.y);
	min.z = min(min.z, v.z);

	max.x = max(max.x, v.x);
	max.y = max(max.y, v.y);
	max.z = max(max.z, v.z);
}

MaloW::Array<MeshStrip*>* ResourceManager::LoadMesh(const char* filePath, ObjData* objData)
{
	MaloW::Array<MeshStrip*>* mesh = NULL;
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
				nrOfVerts++;

				vertpos = objData->faces->get(i).data[2][0] - 1;
				textcoord = objData->faces->get(i).data[2][1] - 1;
				norm = objData->faces->get(i).data[2][2] - 1;
				tempverts[nrOfVerts] = Vertex(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
				DoMinMax(min, max, tempverts[nrOfVerts].pos);
				nrOfVerts++;

				vertpos = objData->faces->get(i).data[1][0] - 1;
				textcoord = objData->faces->get(i).data[1][1] - 1;
				norm = objData->faces->get(i).data[1][2] - 1;
				tempverts[nrOfVerts] = Vertex(objData->vertspos->get(vertpos), objData->textcoords->get(textcoord), objData->vertsnorms->get(norm));
				DoMinMax(min, max, tempverts[nrOfVerts].pos);
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

			mesh->add(strip);
			//this->strips->add(strip);//**tillman**
		}
	}
	delete objData;

	return NULL;
}
*/

ResourceManager::ResourceManager() : gDevice(NULL), gDeviceContext(NULL)
{
	
}

ResourceManager::~ResourceManager()
{
	//Do not release device & device context, this is done by DxManager.

	//Delete any remaining resources
	//Textures
	map<std::string, TextureResource*>::iterator texIterator;
	for(texIterator = this->zTextureResources.begin(); texIterator != this->zTextureResources.end(); texIterator++)
	{
		if(texIterator->second)
		{
			//A texture cannot be deleted since it's destructor is private to force correct use of texture creation/deletion.
			//Instead decrease reference count until it deletes itself.
			int refCount = texIterator->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the resource: " + texIterator->second->GetName() + "; missing decrease(s) in reference counter somewhere. Occurrences: " + MaloW::convertNrToString(refCount));
			for(int i = 0; i < refCount; i++)
			{
				texIterator->second->DecreaseReferenceCount();
			}

			texIterator->second = NULL;
		}
	}
	/*
	//Meshes (strips)
	map<std::string, MeshCounted*>::iterator meshIterator;
	for(meshIterator = this->zMeshes.begin(); meshIterator != this->zMeshes.end(); meshIterator++)
	{
		if(meshIterator->second)
		{
			//A mesh cannot be deleted since it's destructor is private to force correct use of mesh creation/deletion.
			//Instead decrease reference count until it deletes itself.
			int refCount = meshIterator->second->GetReferenceCount();
			MaloW::Debug("WARNING: Resource manager deleted the resource: " + meshIterator->first + "; missing decrease(s) in reference counter somewhere. Occurrences: " + MaloW::convertNrToString(refCount));
			for(int i = 0; i < refCount; i++)
			{
				meshIterator->second->DecreaseReferenceCount();
			}

			meshIterator->second = NULL;
		}
	}*/
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
	//If the shader resource view was not found(already created) in the array, create it.
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
			//Create & Set shader resource view pointer if loading was successful.
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
/*
MeshCounted* ResourceManager::CreateMeshFromFile( const char* filePath )
{
	auto mesh = this->zMeshes.find(filePath);
	//If the mesh was not found in the array, create it.
	if(mesh == this->zMeshes.end())
	{
		//Load data from secondary to primary memory.
		ObjLoader objLoader;
		ObjData* objData = objLoader.LoadObjFile(filePath);

		//If data was transferred (was was successfully read), then load data into classes.
		if(objData)
		{
			//Loads the mesh into array.
			MaloW::Array<MeshStrip*>* meshStrips = this->LoadMesh(filePath);
			//Create & Set loaded mesh.
			this->zMeshes[filePath] = new MeshCounted(meshStrips);
			//Increase reference count.
			this->zMeshes[filePath]->IncreaseReferenceCount();
			
			//Return newly created texture.
			return this->zMeshes[filePath];
		}
		else
		{
			string dbgStr = "WARNING: Failed to load mesh: ";
			dbgStr += filePath;
			MaloW::Debug(dbgStr);

			return NULL;
		}
	}

	//If the mesh already exists, increase reference counter & return mesh.
	this->zMeshes[filePath]->IncreaseReferenceCount();

	return mesh->second;
}*/

void ResourceManager::DeleteTextureResource( TextureResource* &textureResource )
{
	if(textureResource)
	{
		textureResource->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(textureResource->GetReferenceCount() == 1)
		{
			//Find texture.
			auto tex = this->zTextureResources.find(textureResource->GetName());
			//If found..
			if(tex != this->zTextureResources.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				tex->second->DecreaseReferenceCount();
				//Remove texture from table.
				this->zTextureResources.erase(tex);
			}
			textureResource = NULL;
		}
	}
}

/*
void ResourceManager::DeleteMesh( MeshCounted* &meshCounted )
{
	if(meshCounted)
	{
		meshCounted->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(meshCounted->GetReferenceCount() == 1)
		{
			//Find texture.
			auto mesh = this->zMeshes.find(meshCounted->GetName());
			//If found..
			if(mesh != this->zMeshes.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				mesh->second->DecreaseReferenceCount();
				//Remove texture from table.
				this->zMeshes.erase(mesh);
			}
			meshCounted = NULL;
		}
	}
}
*/

void ResourceManager::DeleteBufferResource( BufferResource* &bufferResource )
{

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