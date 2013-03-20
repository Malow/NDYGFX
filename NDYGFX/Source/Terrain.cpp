#include "Terrain.h"
#include "MaloWFileDebug.h"
//#include <vector>

void Terrain::CreateMesh()
{
	//Create vertices
	//this->zNrOfVertices = this->zSize * this->zSize;
	//this->zVertices = new Vertex[this->zSize * this->zSize];

	for(unsigned int i = 0; i < this->zSize; i++)
	{
		for(unsigned int u = 0; u < this->zSize; u++)
		{
			//local pos range [-0.5, 0.5f] * scale
			this->zVertices[i * this->zSize + u] =
				Vertex(	D3DXVECTOR3((float)u / (this->zSize - 1) - 0.5f, 0.0f, (float)i / (this->zSize - 1) - 0.5f), 
						D3DXVECTOR2((float)u / ((this->zSize - 1)),(float)i / ((this->zSize - 1))), 
						D3DXVECTOR3(0.0f, 1.0f, 0.0f),
						D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}


	//Create indices
	//this->zNrOfIndices = (this->zSize - 1) * 2 * (this->zSize - 1) * 3;
	//this->zIndices = new int[this->zNrOfIndices];
	
	int offset = 0; 
	for(unsigned int i = 0; i < this->zSize-1; i++)
	{
		for(unsigned int u = 0; u < this->zSize-1; u++)
		{
			this->zIndices[offset] = i * this->zSize + u;
			offset++;
			this->zIndices[offset] = (i+1) * this->zSize + u;
			offset++;
			this->zIndices[offset] = (i+1) * this->zSize + u + 1; 
			offset++;

			this->zIndices[offset] = i * this->zSize + u;
			offset++;
			this->zIndices[offset] = (i+1) * this->zSize + u + 1;
			offset++;
			this->zIndices[offset] = i * this->zSize + u + 1;
			offset++;
		}
	}
}

//**
void Terrain::CalculateNormals()
{
	for(unsigned int x = 1; x < this->zSize-1; ++x)
	{
		for(unsigned int z = 1; z < this->zSize-1; ++z)
		{
			unsigned int a = (x-1) * this->zSize + z;
			unsigned int b = x * this->zSize + z-1;
			unsigned int d = x * this->zSize + z+1;
			unsigned int e = (x+1) * this->zSize + z;

			D3DXVECTOR3 v1 = this->zVertices[e].pos - this->zVertices[a].pos;
			D3DXVECTOR3 v2 = this->zVertices[b].pos - this->zVertices[d].pos;
			D3DXVECTOR3 norm;
			D3DXVec3Cross(&norm, &v2, &v1);
			D3DXVec3Normalize(&norm,&norm);
			this->zVertices[x * this->zSize + z].normal = norm;
		}
	}

	// Top Normals
	for( unsigned int x = 1; x < this->zSize-1; ++x )
	{
		unsigned int a = x;
		unsigned int b = this->zSize + x - 1;
		unsigned int d = this->zSize + x + 1;

		D3DXVECTOR3 v1 = this->zVertices[b].pos - this->zVertices[a].pos;
		D3DXVECTOR3 v2 = this->zVertices[d].pos - this->zVertices[a].pos;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v1, &v2);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// Bottom Normals
	for( unsigned int x = 1; x < this->zSize-1; ++x )
	{
		unsigned int a = (this->zSize-1) * this->zSize + x;
		unsigned int b = (this->zSize-2) * this->zSize + x - 1;
		unsigned int d = (this->zSize-2) * this->zSize + x + 1;

		D3DXVECTOR3 v1 = this->zVertices[b].pos - this->zVertices[a].pos;
		D3DXVECTOR3 v2 = this->zVertices[d].pos - this->zVertices[a].pos;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v2, &v1);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// Left Normals
	for( unsigned int z = 1; z < this->zSize-1; ++z )
	{
		unsigned int a = this->zSize * z;
		unsigned int b = this->zSize * (z - 1) + 1;
		unsigned int d = this->zSize * (z + 1) + 1;

		D3DXVECTOR3 v1 = this->zVertices[b].pos - this->zVertices[a].pos;
		D3DXVECTOR3 v2 = this->zVertices[d].pos - this->zVertices[a].pos;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v2, &v1);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// Right Normals
	for( unsigned int z = 1; z < this->zSize-1; ++z )
	{
		unsigned int a = this->zSize * z + this->zSize - 1;
		unsigned int b = this->zSize * (z - 1) + this->zSize - 2;
		unsigned int d = this->zSize * (z + 1) + this->zSize - 2;

		D3DXVECTOR3 v1 = this->zVertices[b].pos - this->zVertices[a].pos;
		D3DXVECTOR3 v2 = this->zVertices[d].pos - this->zVertices[a].pos;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v1, &v2);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// TODO: Corners
	zVertices[0].normal = GetNormalAt(1,0) + GetNormalAt(1,1) + GetNormalAt(0,1);
	D3DXVec3Normalize(&zVertices[0].normal,&zVertices[0].normal);

	this->zVertices[zSize-1].normal = GetNormalAt(zSize-2,0) + GetNormalAt(zSize-2,1) + GetNormalAt(zSize-1,1);
	D3DXVec3Normalize(&zVertices[zSize-1].normal,&zVertices[zSize-1].normal);

	this->zVertices[zSize*(zSize-1)].normal = GetNormalAt(0,zSize-2) + GetNormalAt(1,zSize-2) + GetNormalAt(1,zSize-1);
	D3DXVec3Normalize(&zVertices[zSize*(zSize-1)].normal,&zVertices[zSize*(zSize-1)].normal);

	this->zVertices[zSize*zSize-1].normal = GetNormalAt(zSize-2,zSize-1) + GetNormalAt(zSize-2,zSize-2) + GetNormalAt(zSize-1,zSize-2);
	D3DXVec3Normalize(&zVertices[zSize*zSize-1].normal,&zVertices[zSize*zSize-1].normal);
}


Terrain::Terrain()
{
	//Object data
	this->zSize = 0;
	this->zPos = D3DXVECTOR3(0, 0, 0);
	this->zScale = D3DXVECTOR3(0, 0, 0);
	D3DXMatrixIdentity(&this->zWorldMatrix);
	this->RecreateWorldMatrix();
	this->zMaterial = new Material(MaterialType::LAMBERT);

	//Vertex data
	this->zHeightMapHasChanged = false;
	this->zNormalsHaveChanged = false;
	this->zNrOfVertices = 0;
	this->zVertices = NULL;
	this->zVertexBuffer = NULL;
	this->zNrOfIndices = 0;
	this->zIndices = NULL;
	this->zIndexBuffer = NULL;
	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//Texturing
	this->zTextureScale = 1.0f;
	this->zNrOfTextures = 0;
	this->zTextureCapacity = 8;
	this->zTextureResources = new TextureResource*[this->zTextureCapacity];
	this->zTextureResourceToLoadFileName = new string[this->zTextureCapacity];
	for(unsigned int i = 0; i < this->zTextureCapacity; i++)
	{
		this->zTextureResources[i] = NULL;
		//this->zTextureResourceHasChanged[i] = false;
		this->zTextureResourceToLoadFileName[i] = "";
	}
	this->zNrOfBlendMaps = 0;
	this->zBlendMaps = new BlendMap*[this->zTextureCapacity / 4]; //4 textures per blend map.
	for(unsigned int i = 0; i < this->zTextureCapacity / 4; i++)
	{
		this->zBlendMaps[i] = NULL;
	}

	//Collision
	this->zIsCulled = false;
	this->zIsShadowCulled = false;
	this->zRecreateBoundingSphere = false;
	this->zBoundingSphere = BoundingSphere();
	
	//Editor
	this->zUseAIMap = false;
	this->zNrOfAINodesPerSide = 0;
	this->zAIData = NULL;
	this->zAIGridHasChanged = false;
	this->zAIGridShaderResourceView = NULL;
	this->zAIGridThickness = 0.0f;
}



Terrain::Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size)
{
	//Object data
	this->zPos = pos;
	this->zScale = scale;
	this->zSize = size;
	D3DXMatrixIdentity(&this->zWorldMatrix);
	this->RecreateWorldMatrix();
	this->zMaterial = new Material(MaterialType::LAMBERT);

	//Vertex data
	this->zHeightMapHasChanged = false;
	this->zNormalsHaveChanged = false;
	this->zNrOfVertices = this->zSize * this->zSize;
	this->zVertices = new Vertex[this->zSize * this->zSize];
	this->zVertexBuffer = NULL;
	this->zNrOfIndices = (this->zSize - 1) * 2 * (this->zSize - 1) * 3;
	this->zIndices = new int[this->zNrOfIndices];
	this->zIndexBuffer = NULL;
	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//Texturing
	this->zTextureScale = 1.0f;
	this->zNrOfTextures = 0;
	this->zTextureCapacity = 8;
	this->zTextureResources = new TextureResource*[this->zTextureCapacity];
	this->zTextureResourceToLoadFileName = new string[this->zTextureCapacity];
	for(unsigned int i = 0; i < this->zTextureCapacity; i++)
	{
		this->zTextureResources[i] = NULL;
		this->zTextureResourceToLoadFileName[i] = "";
	}
	this->zNrOfBlendMaps = 0;
	this->zBlendMaps = new BlendMap*[this->zTextureCapacity / 4]; //4 textures per blend map.
	for(unsigned int i = 0; i < this->zTextureCapacity / 4; i++)
	{
		this->zBlendMaps[i] = NULL;
	}

	this->CreateMesh();

	//Collision
	this->zIsCulled = false;
	this->zIsShadowCulled = false;
	this->zRecreateBoundingSphere = true;
	this->zBoundingSphere = BoundingSphere();

	//Editor
	this->zUseAIMap = false;
	this->zNrOfAINodesPerSide = 0;
	this->zAIData = NULL;
	this->zAIGridHasChanged = false;
	this->zAIGridShaderResourceView = NULL;
	this->zAIGridThickness = 0.0f;
}

Terrain::~Terrain()
{
	if(this->zVertices) delete this->zVertices; this->zVertices = NULL;
	if(this->zVertexBuffer) delete this->zVertexBuffer; this->zVertexBuffer = NULL;
	if(this->zIndices) delete this->zIndices; this->zIndices = NULL;
	if(this->zIndexBuffer) delete this->zIndexBuffer; this->zIndexBuffer = NULL;

	if(this->zMaterial) delete this->zMaterial; this->zMaterial = NULL;

	if(this->zTextureResources)
	{
		for(unsigned int i = 0; i < this->zNrOfTextures; i++)
		{
			//Decrease reference counter for every texture used
			if(this->zTextureResources[i]) GetResourceManager()->DeleteTextureResource(this->zTextureResources[i]);
		}

		//Delete the array that held them.
		delete [] this->zTextureResources;
		this->zTextureResources = NULL;
	}

	if(this->zTextureResourceToLoadFileName)
	{
		delete [] this->zTextureResourceToLoadFileName;
		this->zTextureResourceToLoadFileName = NULL;
	}

	if(this->zBlendMaps) 
	{
		for(unsigned int i = 0; i < this->zNrOfBlendMaps; i++)
		{
			delete this->zBlendMaps[i];
			this->zBlendMaps[i] = NULL;
		}
		delete [] this->zBlendMaps; 
		this->zBlendMaps = NULL;
	}
	if(this->zAIGridShaderResourceView)
	{
		this->zAIGridShaderResourceView->Release();
		this->zAIGridShaderResourceView = NULL;
	}
}

//OTHER
void Terrain::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->zPos.x, this->zPos.y, this->zPos.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->zScale.x, this->zScale.y, this->zScale.z);

	D3DXMATRIX world = scaling * translate;

	this->zWorldMatrix = world;
}


void Terrain::RecreateBoundingSphere()
{
	//Go through the vertices and find the biggest and smallest values.
	float infinity = std::numeric_limits<float>::infinity();
	D3DXVECTOR3 minPos = D3DXVECTOR3(infinity, infinity, infinity);
	D3DXVECTOR3 maxPos = D3DXVECTOR3(-infinity, -infinity, -infinity);
	for(int i = 0; i < this->zNrOfVertices; i++)
	{
		D3DXVec3Minimize(&minPos, &minPos, &this->zVertices[i].pos);
		D3DXVec3Maximize(&maxPos, &maxPos, &this->zVertices[i].pos);
	}
	//Get the center position and radius from maxPos and minPos.
	this->zBoundingSphere.center = (maxPos + minPos) * 0.5f;
	D3DXVECTOR3 tmp = D3DXVECTOR3(maxPos - this->zBoundingSphere.center);
	this->zBoundingSphere.radius = D3DXVec3Length(&tmp);

	//Finally, set zRecreateBoundingSphere to false so that no unnecessary computation is done next time GetBoundingSphere() is called.
	this->zRecreateBoundingSphere = false;
}



//iTerrain interface functions
//GET
//Vertex data
float Terrain::GetYPositionAt(float x, float z) const
{
	float ex = z / this->zScale.z; //hackfix by swapping z & x
	float ez = x / this->zScale.x;

	if(ex < 1.0f && ez < 1.0f && ex >= 0.0f && ez >= 0.0f) 
	{
		ex *= this->zSize;
		ez *= this->zSize;

		int i = (int)ex;
		int u = (int)ez;

		int a = i * this->zSize + u;
		int b = (i+1) * this->zSize + u;
		int c = i * this->zSize + u+1;
		int d = (i+1) * this->zSize + u+1;
		
		float posya = this->zVertices[i * this->zSize + u].pos.y;
		float posyb = posya;
		float posyc = posya;
		float posyd = posya;

		if(a < (float)(this->zSize * this->zSize))
			posya = this->zVertices[a].pos.y;

		if(b < (float)(this->zSize * this->zSize))
			posyb = this->zVertices[b].pos.y;

		if(c < (float)(this->zSize * this->zSize))
			posyc = this->zVertices[c].pos.y;

		if(d < (float)(this->zSize * this->zSize))
			posyd = this->zVertices[d].pos.y;

		float amem = ((1.0f - ((float)ex - i)) * (1.0f - ((float)ez - u)));
		float bmem = (((float)ex - i) * (1.0f - ((float)ez - u)));
		float cmem = ((1.0f - ((float)ex - i)) * ((float)ez - u));
		float dmem = (((float)ex - i) * ((float)ez - u));

		return posya*amem + posyb*bmem + posyc*cmem + posyd*dmem;
	}
	else
	{
		throw("out of bounds");
	}
}

//SET
//Object data
void Terrain::SetScale(const Vector3& scale)
{
	this->zScale.x = scale.x;
	this->zScale.y = scale.y;
	this->zScale.z = scale.z;
	this->RecreateWorldMatrix();
}
void Terrain::SetDiffuseColor(const Vector3& color )
{
	this->zMaterial->DiffuseColor = D3DXVECTOR3(color.x, color.y, color.z);
}

//Vertex data
void Terrain::SetVertexBuffer(Buffer* vertexBuffer)
{
	if(this->zVertexBuffer) delete this->zVertexBuffer; 
	this->zVertexBuffer = vertexBuffer;
}
void Terrain::SetIndexBuffer(Buffer* indexBuffer)
{
	if(this->zIndexBuffer) delete this->zIndexBuffer; 
	this->zIndexBuffer = indexBuffer;
}

void Terrain::SetHeightMap(float const* const data)
{
	// Update/set y-values of vertices
	for(unsigned int i = 0; i < this->zSize * this->zSize; i++)
	{
		this->zVertices[i].pos.y = data[i];
	}
	
	// Calculate new normals
	// this->CalculateNormals();

	this->zHeightMapHasChanged = true;
	this->zRecreateBoundingSphere = true; //Bounding sphere needs to be recreated (this is done when GetBoundingSphere() is called).
}
void Terrain::SetNormals(float const* const data)
{
	//Update/set normals of vertices
	for(unsigned int i = 0; i < this->zSize * this->zSize * 3; i+=3)
	{
		this->zVertices[i / 3].normal = D3DXVECTOR3(data[i], data[i + 1], data[i + 2]);
	}
	this->zNormalsHaveChanged = true;
}


//Texture
void Terrain::SetTextures(char const* const* const fileNames)
{
	if(fileNames != NULL)
	{
		for(unsigned int i = 0; i < this->zTextureCapacity; i++)
		{
			//Check if any textures are loaded.
			if(this->zTextureResources[i] == NULL || this->zTextureResources[i]->GetName() != string(fileNames[i]))
			{
				this->zTextureResourceToLoadFileName[i] = fileNames[i];
			}
		}
		this->zNrOfTextures = 8; //Hardcoded :S
	}
}
void Terrain::SetBlendMaps(unsigned int nrOfBlendMaps, unsigned int* sizes, float const* const* const data)
{
	if(data != NULL)
	{
		if(nrOfBlendMaps > this->zTextureCapacity / 4)
		{
			MaloW::Debug("WARNING: Terrain: SetBlendMaps: First parameter is too big, truncating to fit.");
			this->zNrOfBlendMaps = this->zTextureCapacity / 4;
		}
		else if(nrOfBlendMaps < this->zTextureCapacity / 4)
		{
			MaloW::Debug("INFO: Terrain: SetBlendMaps: First parameter is too small, not using all blend maps.");
		}

		this->zNrOfBlendMaps = nrOfBlendMaps;
		for(unsigned int i = 0; i < this->zNrOfBlendMaps; i++)
		{
			if(this->zBlendMaps[i] == NULL)
			{
				this->zBlendMaps[i] = new BlendMap();
			}
			//If the size of the blendmap has changed, the texture has to be recreated.
			if(this->zBlendMaps[i]->s_Size != sizes[i])
			{
				this->zBlendMaps[i]->s_RecreateTexture = true;
			}
			this->zBlendMaps[i]->s_Size = sizes[i];
			this->zBlendMaps[i]->s_Data = data[i];
			this->zBlendMaps[i]->s_HasChanged = true;
		}
	}
}


//Collision
//Editor