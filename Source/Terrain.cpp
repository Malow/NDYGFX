#include "Terrain.h"
#include "MaloWFileDebug.h"
//#include <vector>

void Terrain::CreateMesh()
{
	//Create vertices
	int tilingFactor = 1; //**tillman - ändra senare?**

	this->zNrOfVertices = this->zSize * this->zSize;
	this->zVertices = new Vertex[this->zSize * this->zSize];

	for(int i = 0; i < this->zSize; i++)
	//for(int i = this->zSize - 1; i >= 0; i--)
	{
		//for(int u = 0; u < this->zSize; u++)
		for(int u = this->zSize - 1; u >= 0; u--)
		{
			//local pos range [-0.5, 0.5f] * scale
			this->zVertices[i * this->zSize + u] =
				Vertex(	D3DXVECTOR3((float)u / (this->zSize - 1) - 0.5f, 0.0f, (float)i / (this->zSize - 1) - 0.5f), 
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
				D3DXVec3Cross(&norm, &v2, &v1);
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

	this->zTextureScale = 1.0f;
	this->zNrOfTextures = 4;
	this->zTextureFileNames = new string[this->zNrOfTextures];
	this->zTextureFileNames[0] = "";
	this->zTextureFileNames[1] = "";
	this->zTextureFileNames[2] = "";
	this->zTextureFileNames[3] = "";
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

	this->zTextureScale = 1.0f;
	this->zNrOfTextures = 4;
	this->zTextureFileNames = new string[this->zNrOfTextures];
	this->zTextureFileNames[0] = "";
	this->zTextureFileNames[1] = "";
	this->zTextureFileNames[2] = "";
	this->zTextureFileNames[3] = "";
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

	if(this->zTextureFileNames)
	{
		delete[] this->zTextureFileNames;
		this->zTextureFileNames = NULL;
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
float Terrain::GetYPositionAt(float x, float z) const
{
	float ex = z / this->zScale.x; //**tillman - hackfix by swapping z & x **
	float ez = x / this->zScale.z;

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
	int totSize = this->zSize * this->zSize;
	for(int i = 0; i < totSize; i++)
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
			//Check if texture file path has changed
			if(this->zTextureFileNames[i] != string(fileNames[i]))
			{
				//Assign it to the new path if it has
				this->zTextureFileNames[i] = string(fileNames[i]);
			}
		}
	}
}

void Terrain::SetBlendMap(unsigned int size, float const* const data)
{
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