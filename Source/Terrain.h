#ifndef TERRAIN_H
#define TERRAIN_H

#include "iTerrain.h"

#include "Array.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Material.h"

class Terrain : public iTerrain
{
	private:
		int							zSize; //size of mesh (width & height)

		D3DXVECTOR3					zPos;
		D3DXVECTOR3					zScale;
		D3DXMATRIX					zWorldMatrix;

		int							zNrOfVertices;
		Vertex*						zVertices;	
		Buffer*						zVertexBuffer;
		int							zNrOfIndices;
		int*						zIndices; 
		Buffer*						zIndexBuffer;

		Material*					zMaterial;
		D3D_PRIMITIVE_TOPOLOGY		zTopology;
		
		string						zTex1;
		string						zTex2;
		string						zTex3;
		ID3D11ShaderResourceView*	zSRV1;
		ID3D11ShaderResourceView*	zSRV2;
		ID3D11ShaderResourceView*	zSRV3;

	private:
		void CreateMesh();
		//Calculates normals for each vertex in the height map
		void CalculateNormals();


	public:
		Terrain();
		Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size);
		virtual ~Terrain();

		//Get
		D3DXVECTOR3 GetPosition() const { return this->zPos; } 
		D3DXVECTOR3 GetScale() const { return this->zScale; }
		D3DXMATRIX GetWorldMatrix() const { return this->zWorldMatrix; }

		int GetNrOfVertices() const { return this->zNrOfVertices; }
		Vertex* GetVertices() { return this->zVertices; }
		Buffer* GetVertexBuffer() { return this->zVertexBuffer; }
		int GetNrOfIndices() const { return this->zNrOfIndices; }
		int* GetIndices() { return this->zIndices; }
		Buffer* GetIndexBuffer() { return this->zIndexBuffer; }

		Material* GetMaterial() const { return this->zMaterial; }
		D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }

		string GetTexture1() { return zTex1; } //**temp**
		string GetTexture2() { return zTex2; }//**temp**
		string GetTexture3() { return zTex3; }//**temp**
		ID3D11ShaderResourceView* GetSRV1() { return zSRV1; }//**temp**
		ID3D11ShaderResourceView* GetSRV2() { return zSRV2; }//**temp**
		ID3D11ShaderResourceView* GetSRV3() { return zSRV3; }//**temp**

		//Set
		void SetVertexBuffer(Buffer* vertexBuffer);
		void SetIndexBuffer(Buffer* indexBuffer);
		//void SetSRV1**

		// Is used internally when needed, but can be used from the outside for debugging.
		void RecreateWorldMatrix();
		
		//bool LoadTexture();
		//bool LoadTextures()

		//iTerrain interface functions
		virtual float GetYPositionAt(float x, float z); 
		virtual bool SetHeightMap(float* data); 
		virtual bool SetTextures(const char* fileName1 = NULL, const char* fileName2 = NULL, const char* fileName3 = NULL);
		virtual bool SetBlendMap(unsigned int size, float* data);



};

#endif