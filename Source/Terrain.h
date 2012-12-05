#ifndef TERRAIN_H
#define TERRAIN_H

#include "iTerrain.h"

#include "Array.h"
#include "Vertex.h"
#include "Buffer.h"

class Terrain : public iTerrain
{
	private:
		int							zSize; //size of mesh (width & height)

		D3DXVECTOR3					zPos;
		D3DXVECTOR3					zScale;
		D3DXMATRIX					zWorldMatrix;

		D3D_PRIMITIVE_TOPOLOGY		zTopology;
		Vertex*						zMesh;	
		int							zNrOfVertices;
		Buffer*						zVertexBuffer;
		int							zNrOfIndices;
		Buffer*						zIndexBuffer;
		
		ID3D11Texture2D*			zRChannelTex;
		ID3D11Texture2D*			zGChannelTex;
		ID3D11Texture2D*			zBChannelTex;
		ID3D10ShaderResourceView*	zRSRV;
		ID3D10ShaderResourceView*	zGSRV;
		ID3D10ShaderResourceView*	zBSRV;

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
		D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }
		/*
		// Is used internally when needed, but can be used from the outside for debugging.
		void RecreateWorldMatrix();
		*/
		bool LoadTexture();
		//bool LoadTextures()

		//iTerrain interface functions
		virtual float GetYPositionAt(float x, float z); 
		virtual bool SetHeightMap(unsigned int size, float* data); //size^2 = size of array
		virtual bool SetTextures(const char* fileName1 = NULL, const char* fileName2 = NULL, const char* fileName3 = NULL);
		virtual bool SetBlendMap(unsigned int size, float* data);



};

#endif