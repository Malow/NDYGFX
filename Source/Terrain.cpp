#include "Terrain.h"
#include "MaloWFileDebug.h"
//#include <vector>

void Terrain::CreateMesh()
{
	//Create vertices
	int tilingFactor = 1; //**ändra senare**

	this->zNrOfVertices = this->zSize * this->zSize;
	this->zVertices = new Vertex[this->zSize * this->zSize];

	for(int i = 0; i < this->zSize; i++)
	{
		for(int u = 0; u < this->zSize; u++)
		{
			//local pos range [-0.5, 0.5f] * scale
			this->zVertices[i * this->zSize + u] = 
				Vertex(	D3DXVECTOR3((float)i / (this->zSize - 1) - 0.5f, 0.0f, (float)u / (this->zSize - 1) - 0.5f), 
						D3DXVECTOR2((float)i / ((this->zSize - 1) / tilingFactor), (float)u / ((this->zSize - 1) / tilingFactor)), 
						D3DXVECTOR3(0, 1, 0),
						D3DXVECTOR3(0, 0, 0));
		}
	}


	//Create indices
	this->zNrOfIndices = (this->zSize - 1) * 2 * (this->zSize - 1) * 3;
	this->zIndices = new int[this->zNrOfIndices];
	
	int offset = 0; 
	for(int i = 0; i < this->zSize-1; i++)
	{
		for(int u = 0; u < this->zSize-1; u++)
		{
			this->zIndices[offset] = i * this->zSize + u;
			offset++;
			this->zIndices[offset] = (i+1) * this->zSize + u + 1; 
			offset++;
			this->zIndices[offset] = (i+1) * this->zSize + u; 
			offset++;

			this->zIndices[offset] = i * this->zSize + u;
			offset++;
			this->zIndices[offset] = i * this->zSize + u + 1; 
			offset++;
			this->zIndices[offset] = (i+1) * this->zSize + u + 1; 
			offset++;
		}
	}
}

//**
void Terrain::CalculateNormals()
{
	for(int q = 0; q < this->zSize; q++)
	{
		for(int u = 0; u < this->zSize; u++)
		{
			int a = (q-1) * this->zSize + u;
			int b = q * this->zSize + u-1;
			int d = q * this->zSize + u+1;
			int e = (q+1) * this->zSize + u;

			if(q != 0 && u != 0 && u != this->zSize-1 && q != this->zSize-1) //**tillman optimera**
			{
				D3DXVECTOR3 v1 = this->zVertices[e].pos - this->zVertices[a].pos;
				D3DXVECTOR3 v2 = this->zVertices[b].pos - this->zVertices[d].pos;
				D3DXVECTOR3 norm;
				D3DXVec3Cross(&norm, &v1, &v2);
				this->zVertices[q * this->zSize + u].normal = norm;
			}
		}
	}
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

	this->zTextures = new Texture*[3];
	this->zTextures[0] = NULL;
	this->zTextures[1] = NULL;
	this->zTextures[2] = NULL;
	this->zBlendMap = new BlendMap();
	
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

	this->zTextures = new Texture*[3];
	this->zTextures[0] = NULL;
	this->zTextures[1] = NULL;
	this->zTextures[2] = NULL;
	this->zBlendMap = new BlendMap();

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
		if(this->zTextures[0]) delete this->zTextures[0]; this->zTextures[0] = NULL;
		if(this->zTextures[1]) delete this->zTextures[1]; this->zTextures[1] = NULL;
		if(this->zTextures[2]) delete this->zTextures[2]; this->zTextures[2] = NULL;

		delete[] this->zTextures;
		this->zTextures = NULL;
	}
	if(this->zBlendMap) delete this->zBlendMap; this->zBlendMap = NULL;
}


//Get

//Set

//Other
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
float Terrain::GetYPositionAt(float x, float z)
{
	x -= this->zPos.x;
	z -= this->zPos.z;

	float ex = x;
	//ex /= this->dimensions.x;
	float ez = z;
	//ez /= this->dimensions.z;


	if(ex <= 1.0f && ez <= 1.0f && ex > 0.0f && ez > 0.0f) //**tillman opt
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

		if(a < this->zSize * this->zSize)
			posya = this->zVertices[a].pos.y;

		if(b < this->zSize * this->zSize)
			posyb = this->zVertices[b].pos.y;

		if(c < this->zSize * this->zSize)
			posyc = this->zVertices[c].pos.y;

		if(d < this->zSize * this->zSize)
			posyd = this->zVertices[d].pos.y;

		float amem = ((1.0f - ((float)ex - i)) * (1.0f - ((float)ez - u)));
		float bmem = (((float)ex - i) * (1.0f - ((float)ez - u)));
		float cmem = ((1.0f - ((float)ex - i)) * ((float)ez - u));
		float dmem = (((float)ex - i) * ((float)ez - u));

		return posya*amem + posyb*bmem + posyc*cmem + posyd*dmem;
	}
	else
		return 10.0f;
}

//Set
void Terrain::SetScale(Vector3& scale)
{
	this->zScale.x = scale.x;
	this->zScale.y = scale.y;
	this->zScale.z = scale.z;
	this->RecreateWorldMatrix();
}

void Terrain::SetHeightMap(float* data)
{
	//Update/set y-values of vertices
	int totSize = this->zSize * this->zSize;
	for(int i = 0; i < totSize; i++)
	{
		this->zVertices[i].pos.y = data[i];
	}
	
	//Calculate new normals
	this->CalculateNormals();

	this->zHeightMapHasChanged = true;
}

void Terrain::SetTextures(const char** fileNames)
{
	if(fileNames)
	{
		for(int i = 0; i < 3; i++)
		{
			if(this->zTextures[i])
			{
				if(this->zTextures[i]->FileName == string(fileNames[i]))
				{
					this->zTextures[i]->FileName  = fileNames[i];
					this->zTextures[i]->HasChanged = true;
				}
			}
			else if(fileNames[i])
			{
				this->zTextures[i] = new Texture(fileNames[i]);
			}
		}
	}
}

void Terrain::SetBlendMap(unsigned int size, float* data)
{
	this->zBlendMap->Size = size;
	this->zBlendMap->Data = data;
	this->zBlendMap->HasChanged = true;
}
