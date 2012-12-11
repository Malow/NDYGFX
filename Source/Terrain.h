#ifndef TERRAIN_H
#define TERRAIN_H

#include "iTerrain.h"

#include "Array.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Material.h"


struct Texture
{
	bool						HasChanged;
	string						FileName;
	ID3D11ShaderResourceView*	SRV;

	Texture() {HasChanged = true; FileName = ""; SRV = NULL; }
	Texture(string fileName) {HasChanged = true; FileName = fileName; SRV = NULL; }
	virtual ~Texture() { if(SRV) SRV->Release(); SRV = NULL; }
};
struct BlendMap
{
	bool						HasChanged;
	unsigned int				Size;
	float*						Data;
	ID3D11ShaderResourceView*	SRV;
	
	BlendMap() {HasChanged = false; Size = 0; Data = NULL;  SRV = NULL; }
	BlendMap(unsigned int size, float* data) {HasChanged = true; Size = size; Data = data; SRV = NULL; }
	virtual ~BlendMap() { Data = NULL; if(SRV) SRV->Release(); SRV = NULL; }

};

class Terrain : public iTerrain
{
	private:
		

	private:
		int							zSize; //size of mesh (width & height)

		D3DXVECTOR3					zPos;
		D3DXVECTOR3					zScale;
		D3DXMATRIX					zWorldMatrix;

		bool						zHeightMapHasChanged;
		int							zNrOfVertices;
		Vertex*						zVertices;	
		Buffer*						zVertexBuffer;
		int							zNrOfIndices;
		int*						zIndices; 
		Buffer*						zIndexBuffer;

		Material*					zMaterial;
		D3D_PRIMITIVE_TOPOLOGY		zTopology;
		
		Texture**					zTextures;
		BlendMap*					zBlendMap;
		

	private:
		void CreateMesh();
		//Calculates normals for each vertex in the height map
		void CalculateNormals();


	public:
		Terrain();
		Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size);
		virtual ~Terrain();

		//Get
		int GetSize() const { return this->zSize; }
		D3DXVECTOR3 GetPosition() const { return this->zPos; } 
		D3DXVECTOR3 GetScale() const { return this->zScale; }
		D3DXMATRIX GetWorldMatrix() const { return this->zWorldMatrix; }

		bool HasHeightMapChanged() const { return this->zHeightMapHasChanged; }
		int GetNrOfVertices() const { return this->zNrOfVertices; }
		Vertex* GetVerticesPointer() const { return this->zVertices; }
		Buffer* GetVertexBufferPointer() const { return this->zVertexBuffer; }
		int GetNrOfIndices() const { return this->zNrOfIndices; }
		int* GetIndicesPointer() const { return this->zIndices; }
		Buffer* GetIndexBufferPointer() const { return this->zIndexBuffer; }

		Material* GetMaterial() const { return this->zMaterial; }
		D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }

		Texture* GetTexturePointer(unsigned int index) { return this->zTextures[index]; }
		BlendMap* GetBlendMapPointer() { return this->zBlendMap; }

		//Set
		void SetScale(D3DXVECTOR3 scale) { this->zScale = scale; }
		void HeightMapHasChanged(bool has) { this->zHeightMapHasChanged = has; }
		void SetVertexBuffer(Buffer* vertexBuffer) { this->zVertexBuffer = vertexBuffer; }
		void SetIndexBuffer(Buffer* indexBuffer) { this->zIndexBuffer = indexBuffer; }
		//void SetSRV1**

		//Other
		//Is used internally when needed, but can be used from the outside for debugging.
		void RecreateWorldMatrix();
		
		//bool LoadTexture();
		//bool LoadTextures()



		//iTerrain interface functions
		//Get
		virtual float GetYPositionAt(float x, float z); 

		//Set
		virtual void SetScale(Vector3& scale);
		virtual void SetHeightMap(float* data); 
		virtual void SetTextures(const char** fileNames);
		virtual void SetBlendMap(unsigned int size, float* data);



};

#endif