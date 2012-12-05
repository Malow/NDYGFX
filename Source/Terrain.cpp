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
				D3DXVECTOR3 v;
				D3DXVec3Cross(&v, &D3DXVECTOR3(v1.x, v1.y, v1.z), &D3DXVECTOR3(v2.x, v2.y, v2.z));
				//Vector3 v = Vector3(v1.x, v1.y, v1.z).GetCrossProduct(Vector3(v2.x, v2.y, v2.z)); //old
				this->zVertices[q * this->zSize + u].normal = D3DXVECTOR3(v.x, v.y, v.z);
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

	this->zNrOfVertices = 0;
	this->zVertices = NULL;
	this->zVertexBuffer = NULL;
	this->zNrOfIndices = 0;
	this->zIndices = NULL;
	this->zIndexBuffer = NULL;

	this->zMaterial = new Material(MaterialType::LAMBERT);
	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	this->zTex1 = "";
	this->zTex2 = "";
	this->zTex3 = "";
	this->zSRV1 = NULL;
	this->zSRV2 = NULL;
	this->zSRV3 = NULL;
	
}



Terrain::Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size)
{
	this->zPos = pos;
	this->zScale = scale;
	this->zSize = size;
	D3DXMatrixIdentity(&this->zWorldMatrix);
	this->RecreateWorldMatrix();

	this->zNrOfVertices = 0;
	this->zVertices = NULL;
	this->zVertexBuffer = NULL;
	this->zNrOfIndices = 0;
	this->zIndices = NULL;
	this->zIndexBuffer = NULL;

	this->zMaterial = new Material(MaterialType::LAMBERT);
	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	this->zTex1 = "";
	this->zTex2 = "";
	this->zTex3 = "";
	this->zSRV1 = NULL;
	this->zSRV2 = NULL;
	this->zSRV3 = NULL;

	this->CreateMesh();
}

Terrain::~Terrain()
{
	if(this->zVertices) delete this->zVertices; this->zVertices = NULL;
	if(this->zVertexBuffer) delete this->zVertexBuffer; this->zVertexBuffer = NULL;
	if(this->zIndices) delete this->zIndices; this->zIndices = NULL;
	if(this->zIndexBuffer) delete this->zIndexBuffer; this->zIndexBuffer = NULL;

	if(this->zMaterial) delete this->zMaterial; this->zMaterial = NULL;

	if(this->zSRV1) this->zSRV1->Release();
	if(this->zSRV2) this->zSRV2->Release();
	if(this->zSRV3) this->zSRV3->Release();
}


//Get

//Set
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

//iTerrain interface functions
float Terrain::GetYPositionAt(float x, float z)
{
	x -= this->zPos.x;
	z -= this->zPos.z;

	float ex = x;
	//ex /= this->dimensions.x;
	float ez = z;
	//ez /= this->dimensions.z;


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
bool Terrain::SetHeightMap(float* data)
{
	//Update/set y-values of vertices
	int totSize = this->zSize * this->zSize;
	for(int i = 0; i < totSize; i++)
	{
		this->zVertices[i].pos.y = data[i];
	}

	//calculate new normals
	this->CalculateNormals();


	return true;
}

bool Terrain::SetTextures(const char* fileName1, const char* fileName2, const char* fileName3)
{
	this->zTex1 = fileName1;
	this->zTex2 = fileName2;
	this->zTex3 = fileName3;
	//**TODO: reload textures**

	return true;
}

bool Terrain::SetBlendMap(unsigned int size, float* data)
{
	return true;
}











/*

void Terrain::SetPosition(D3DXVECTOR3 pos)
{ 
	this->zPos = pos;
	this->RecreateWorldMatrix();
}

void Terrain::SetQuaternion(D3DXQUATERNION quat)
{
	this->zRotQuat = quat;
	this->RecreateWorldMatrix();
}


void Terrain::MoveBy(D3DXVECTOR3 moveby)
{ 
	this->pos += moveby; 
	this->RecreateWorldMatrix();
}

void Terrain::MoveBy( const Vector3& moveby )
{
	this->MoveBy( D3DXVECTOR3(moveby.x,moveby.y,moveby.z) );
}

void Terrain::Rotate(D3DXVECTOR3 radians)
{
	D3DXQUATERNION quaternion;
	D3DXQuaternionRotationYawPitchRoll(&quaternion, radians.y, radians.x, radians.z);
	D3DXQuaternionMultiply(&this->rotQuat, &this->rotQuat, &quaternion);
	this->RecreateWorldMatrix();
}

void Terrain::Rotate( const Vector3& radians )
{
	this->Rotate( D3DXVECTOR3(radians.x,radians.y,radians.z) );
}

void Terrain::Rotate(D3DXQUATERNION quat)
{
	D3DXQuaternionMultiply(&this->rotQuat, &this->rotQuat, &quat);
	this->RecreateWorldMatrix();
}

void Terrain::Rotate( const Vector4& quat )
{
	this->Rotate( D3DXQUATERNION(quat.x,quat.y,quat.z,quat.w) );
}

void Terrain::RotateAxis(D3DXVECTOR3 around, float angle)
{
	D3DXQUATERNION quaternion = D3DXQUATERNION(0, 0, 0, 1);
	D3DXQuaternionRotationAxis(&quaternion, &around, angle);
	
	D3DXQuaternionMultiply(&this->rotQuat, &this->rotQuat, &quaternion);
	this->RecreateWorldMatrix();
}

void Terrain::RotateAxis( const Vector3& around, float angle )
{
	this->RotateAxis( D3DXVECTOR3(around.x,around.y,around.z), angle );
}

void Terrain::Scale(D3DXVECTOR3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
	this->RecreateWorldMatrix();
}

void Terrain::Scale( const Vector3& scale )
{
	this->Scale( D3DXVECTOR3(scale.x,scale.y,scale.z) );
}

void Terrain::Scale(float scale)
{
	this->scale.x *= scale;
	this->scale.y *= scale;
	this->scale.z *= scale;
	this->RecreateWorldMatrix();
}

void Terrain::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->pos.x, this->pos.y, this->pos.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->scale.x, this->scale.y, this->scale.z);

	
	// Euler
	//D3DXMATRIX x, y, z;
	//D3DXMatrixRotationX(&x, this->rot.x);
	//D3DXMatrixRotationY(&y, this->rot.y);
	//D3DXMatrixRotationZ(&z, this->rot.z);

	//D3DXMATRIX world = scaling*x*y*z*translate;
	

	// Quaternions
	D3DXMATRIX QuatMat;
	D3DXMatrixRotationQuaternion(&QuatMat, &this->rotQuat); 

	D3DXMATRIX world = scaling*QuatMat*translate;




	this->worldMatrix = world;
}

void Terrain::ResetRotationAndScale()
{
	this->rotQuat = D3DXQUATERNION(0, 0, 0, 1);
	this->scale = D3DXVECTOR3(1, 1, 1);
	this->RecreateWorldMatrix();
}


*/






/* //iTerrain interface functions
void Terrain::SetPosition( const Vector3& pos )
{
	this->SetPosition( D3DXVECTOR3(pos.x,pos.y,pos.z) );
}

void Terrain::SetQuaternion( const Vector4& quat )
{
	this->SetQuaternion( D3DXQUATERNION(quat.x,quat.y,quat.z,quat.w) );
}

Vector3 Terrain::GetPosition() const
{
	return Vector3(this->pos.x, this->pos.y, this->pos.z);
}

Vector3 Terrain::GetRotation() const
{
	D3DXVECTOR3 rotxyz;
	float angle = 0.0f;
	D3DXQuaternionToAxisAngle(&this->rotQuat, &rotxyz, &angle);	// Return rotation around an axis by angle
	// convert rotxyz to actual xyz rotations, DOESNT WORK YET!
	return Vector3(rotxyz.x, rotxyz.y, rotxyz.z);
}

Vector3 Terrain::GetScaling() const
{
	return Vector3(this->scale.x, this->scale.y, this->scale.z);
}
*/






































/*
bool Terrain::LoadAndApplyHeightMap(const char* fileName)
{
	std::ifstream fin;
	fin.open(fileName, ios_base::binary);
	if(!fin)
	{
		MaloW::Debug("Height map for terrain could not be found");
		return false;
	}

	std::vector<unsigned char> vertexHeights(this->SIZE*this->SIZE);

	fin.read((char *)&vertexHeights[0], (streamsize)vertexHeights.size());
	fin.close();

	Vertex* verts = this->strips->get(0)->getVerts();
	for(int i = 0; i < (int)vertexHeights.size(); i++)
	{
		verts[i].pos.y *= (float)vertexHeights[i]/10;
	}
	return true;
}

void Terrain::filter(unsigned int smootheness)
{
	for(unsigned int smoothe = 0; smoothe < smootheness; smoothe++)
	{
		Vertex** grid = new Vertex*[this->SIZE];
		for(int i = 0; i < this->SIZE; i++)
			grid[i] = new Vertex[this->SIZE];

		Vertex* verts = this->strips->get(0)->getVerts();

		for(int i = 0; i < this->SIZE; i++)
			for(int u = 0; u < this->SIZE; u++)
				grid[i][u] = verts[i * this->SIZE + u];


		for(int q = 0; q < this->SIZE; q++)
		{
			for(int u = 0; u < this->SIZE; u++)
			{
				int a = (q-1)*this->SIZE + u-1;
				int b = (q-1)*this->SIZE + u;
				int c = (q-1)*this->SIZE + u+1;
				int d = q*this->SIZE + u-1;
				int e = q*this->SIZE + u;		// Urself
				int f = q*this->SIZE + u+1;
				int g = (q+1)*this->SIZE + u-1;
				int h = (q+1)*this->SIZE + u;
				int i = (q+1)*this->SIZE + u+1;
				if(q == 0 && u == 0)
				{
					grid[q][u].pos.y = (verts[e].pos.y + verts[f].pos.y + verts[h].pos.y + verts[i].pos.y) / 4;
				}
				else if(q == 0 && u == this->SIZE-1)
				{
					grid[q][u].pos.y = (verts[d].pos.y + verts[e].pos.y + verts[g].pos.y + verts[h].pos.y) / 4;
				}
				else if(q == this->SIZE-1 && u == 0)
				{
					grid[q][u].pos.y = (verts[b].pos.y + verts[c].pos.y + verts[e].pos.y + verts[f].pos.y) / 4;
				}
				else if(q == this->SIZE-1 && u == this->SIZE-1)
				{
					grid[q][u].pos.y = (verts[a].pos.y + verts[b].pos.y + verts[d].pos.y + verts[e].pos.y) / 4;
				}
				else if(q == 0)
				{
					grid[q][u].pos.y = (verts[d].pos.y + verts[e].pos.y + verts[f].pos.y + verts[g].pos.y + verts[h].pos.y + verts[i].pos.y) / 6;
				}
				else if(q == this->SIZE-1)
				{
					grid[q][u].pos.y = (verts[a].pos.y + verts[b].pos.y + verts[c].pos.y + verts[d].pos.y + verts[e].pos.y + verts[f].pos.y) / 6;
				}
				else if(u == 0)
				{
					grid[q][u].pos.y = (verts[b].pos.y + verts[c].pos.y + verts[e].pos.y + verts[f].pos.y + verts[i].pos.y + verts[h].pos.y) / 6;
				}
				else if(u == this->SIZE-1)
				{
					grid[q][u].pos.y = (verts[a].pos.y + verts[b].pos.y + verts[d].pos.y + verts[e].pos.y + verts[g].pos.y + verts[h].pos.y) / 6;
				}
				else
				{
					grid[q][u].pos.y = (verts[a].pos.y + verts[b].pos.y + verts[c].pos.y + verts[d].pos.y + verts[e].pos.y + verts[f].pos.y
						+ verts[g].pos.y + verts[h].pos.y + verts[i].pos.y ) / 9;
				}
			}
		}

		for(int i = 0; i < this->SIZE; i++)
			for(int u = 0; u < this->SIZE; u++)
				verts[i * this->SIZE + u] = grid[i][u];
	
		for(int i = 0; i < this->SIZE; i++)
			delete [] grid[i];

		delete grid;
	}
}
*/