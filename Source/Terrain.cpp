#include "Terrain.h"
#include "MaloWFileDebug.h"
//#include <vector>

void Terrain::CreateMesh()
{
	//Create vertices
	int tilingFactor = 1; //**tillman - ändra senare?**

	this->zNrOfVertices = this->zSize * this->zSize;
	this->zVertices = new Vertex[this->zSize * this->zSize];

	for(unsigned int i = 0; i < this->zSize; i++)
	{
		for(unsigned int u = 0; u < this->zSize; u++)
		{
			//local pos range [-0.5, 0.5f] * scale
			this->zVertices[i * this->zSize + u] =
				Vertex(	D3DXVECTOR3((float)u / (this->zSize - 1) - 0.5f, 0.0f, (float)i / (this->zSize - 1) - 0.5f), 
						D3DXVECTOR2((float)u / ((this->zSize - 1) / tilingFactor),(float)i / ((this->zSize - 1) / tilingFactor)), 
						D3DXVECTOR3(0.0f, 1.0f, 0.0f),
						D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}


	//Create indices
	this->zNrOfIndices = (this->zSize - 1) * 2 * (this->zSize - 1) * 3;
	this->zIndices = new int[this->zNrOfIndices];
	
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
	this->zVertices[0].normal = D3DXVECTOR3(0.0f,1.0f,0.0f);
	this->zVertices[zSize-1].normal = D3DXVECTOR3(0.0f,1.0f,0.0f);
	this->zVertices[zSize*(zSize-1)].normal = D3DXVECTOR3(0.0f,1.0f,0.0f);
	this->zVertices[zSize*zSize-1].normal = D3DXVECTOR3(0.0f,1.0f,0.0f);
}


Terrain::Terrain()
{
	this->zSize = 0;
	this->zPos = D3DXVECTOR3(0, 0, 0);
	this->zScale = D3DXVECTOR3(0, 0, 0);
	D3DXMatrixIdentity(&this->zWorldMatrix);
	this->RecreateWorldMatrix();

	this->zHeightMapHasChanged = false;
	this->zNrOfVertices = 0;
	this->zVertices = NULL;
	this->zVertexBuffer = NULL;
	this->zNrOfIndices = 0;
	this->zIndices = NULL;
	this->zIndexBuffer = NULL;

	this->zMaterial = new Material(MaterialType::LAMBERT);
	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	this->zTextureScale = 1.0f;
	this->zNrOfTextures = 4;
	this->zTextures = new Texture*[this->zNrOfTextures];
	this->zTextures[0] = NULL;
	this->zTextures[1] = NULL;
	this->zTextures[2] = NULL;
	this->zTextures[3] = NULL;
	this->zBlendMap = NULL;
}



Terrain::Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size)
{
	this->zPos = pos;
	this->zScale = scale;
	this->zSize = size;
	D3DXMatrixIdentity(&this->zWorldMatrix);
	this->RecreateWorldMatrix();

	this->zHeightMapHasChanged = false;
	this->zNrOfVertices = 0;
	this->zVertices = NULL;
	this->zVertexBuffer = NULL;
	this->zNrOfIndices = 0;
	this->zIndices = NULL;
	this->zIndexBuffer = NULL;

	this->zMaterial = new Material(MaterialType::LAMBERT);
	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	this->zTextureScale = 1.0f;
	this->zNrOfTextures = 4;
	this->zTextures = new Texture*[this->zNrOfTextures];
	this->zTextures[0] = NULL;
	this->zTextures[1] = NULL;
	this->zTextures[2] = NULL;
	this->zTextures[3] = NULL;
	this->zBlendMap = NULL;

	this->CreateMesh();
}

Terrain::~Terrain()
{
	if(this->zVertices) delete this->zVertices; this->zVertices = NULL;
	if(this->zVertexBuffer) delete this->zVertexBuffer; this->zVertexBuffer = NULL;
	if(this->zIndices) delete this->zIndices; this->zIndices = NULL;
	if(this->zIndexBuffer) delete this->zIndexBuffer; this->zIndexBuffer = NULL;

	if(this->zMaterial) delete this->zMaterial; this->zMaterial = NULL;

	if(this->zTextures)
	{
		//Decrease reference counter for every texture used
		if(this->zTextures[0]) GetResourceManager()->DeleteTexture(this->zTextures[0]);
		if(this->zTextures[1]) GetResourceManager()->DeleteTexture(this->zTextures[1]);
		if(this->zTextures[2]) GetResourceManager()->DeleteTexture(this->zTextures[2]);
		if(this->zTextures[3]) GetResourceManager()->DeleteTexture(this->zTextures[3]);

		//Delete the array that held them.
		delete [] this->zTextures;
		this->zTextures = NULL;
	}
	if(this->zBlendMap) delete this->zBlendMap; this->zBlendMap = NULL;
}


void Terrain::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->zPos.x, this->zPos.y, this->zPos.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->zScale.x, this->zScale.y, this->zScale.z);

	D3DXMATRIX world = scaling * translate;

	this->zWorldMatrix = world;
}



//iTerrain interface functions
float Terrain::GetYPositionAt(float x, float z) const
{
	float ex = z / this->zScale.z; //**tillman - hackfix by swapping z & x **
	float ez = x / this->zScale.x;

	//if(((ex && ex) <= 1.0f) && ((ex && ez) > 0.0f)) //**tillman opt
	if(ex <= 1.0f && ez <= 1.0f && ex > 0.0f && ez > 0.0f) 
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

//Set
void Terrain::SetScale(const Vector3& scale)
{
	this->zScale.x = scale.x;
	this->zScale.y = scale.y;
	this->zScale.z = scale.z;
	this->RecreateWorldMatrix();
}

void Terrain::SetHeightMap(float const* const data)
{
	//Update/set y-values of vertices
	for(unsigned int i = 0; i < this->zSize * this->zSize; i++)
	{
		this->zVertices[i].pos.y = data[i];
	}
	
	//Calculate new normals
	this->CalculateNormals();

	this->zHeightMapHasChanged = true;
}

void Terrain::SetTextures(char const* const* const fileNames)
{
	if(fileNames)
	{
		for(int i = 0; i < this->zNrOfTextures; i++)
		{
			//Check if any textures are loaded.
			if(this->zTextures[i] == NULL)
			{
				this->zTextures[i] = GetResourceManager()->CreateTextureFromFile(fileNames[i]);
			}
			//Check if texture file path has changed
			else if(this->zTextures[i]->GetName() != string(fileNames[i]))
			{
				//Decrease the reference
				GetResourceManager()->DeleteTexture(this->zTextures[i]);
				//Assign it to the new path if it has
				this->zTextures[i] = GetResourceManager()->CreateTextureFromFile(fileNames[i]);
			}
		}
	}
}

void Terrain::SetBlendMap(unsigned int size, float const* const data)
{
	if(this->zBlendMap == NULL)
	{
		this->zBlendMap = new BlendMap();
	}
	this->zBlendMap->Size = size;
	this->zBlendMap->Data = data;
	this->zBlendMap->HasChanged = true;
}

void Terrain::SetDiffuseColor(const Vector3& color )
{
	this->zMaterial->DiffuseColor = D3DXVECTOR3(color.x, color.y, color.z);
}

void Terrain::SetTextureScale(float textureScale)
{
	this->zTextureScale = textureScale;
}


Vector3 Terrain::GetPosition() const
{
	return Vector3(zPos.x,zPos.y,zPos.z);
}