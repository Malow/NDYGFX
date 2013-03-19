/*



*/

#ifndef TERRAIN_H
#define TERRAIN_H

#include "iTerrain.h"

#include "Array.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Material.h"
#include "TextureResource.h"
#include "ResourceManager.h"
#include "BoundingSphere.h"


struct BlendMap
{
	bool						s_HasChanged;
	bool						s_RecreateTexture;
	unsigned int				s_Size;
	float const*				s_Data; //values in data shall not be changed, only the pointer.
	ID3D11Texture2D*			s_Texture; //Texture containing Data
	ID3D11ShaderResourceView*	s_SRV; //Shader resource view containing Texture.
	
	BlendMap() : s_HasChanged(false), s_RecreateTexture(false), s_Size(0), s_Data(NULL), s_Texture(NULL), s_SRV(NULL) {}
	BlendMap(unsigned int size, float* data) 
	: s_HasChanged(true), s_RecreateTexture(true), s_Size(size), s_Data(data), s_Texture(NULL), s_SRV(NULL) {} 
	virtual ~BlendMap() { s_Data = NULL; if(s_Texture) s_Texture->Release(); if(s_SRV) s_SRV->Release(); s_SRV = NULL; }

};/*
struct BoundingBox
{
	Vector3 Size;
	Vector3 MinPos;
	Vector3 MaxPos; 

	BoundingBox(const Vector3 &Size = Vector3(), const Vector3 &MinPos = Vector3(), const Vector3 &MaxPos = Vector3()) : Size(Size), MinPos(MinPos), MaxPos(MaxPos) {}
};*/

class Terrain : public iTerrain
{
	private:
		//Object data
		unsigned int				zSize; //size of mesh (width & height)
		D3DXVECTOR3					zPos;
		D3DXVECTOR3					zScale;
		D3DXMATRIX					zWorldMatrix;
		Material*					zMaterial;

		//Vertex data
		bool						zHeightMapHasChanged;
		bool						zNormalsHaveChanged;
		int							zNrOfVertices;
		Vertex*						zVertices;	
		Buffer*						zVertexBuffer;
		int							zNrOfIndices;
		int*						zIndices; 
		Buffer*						zIndexBuffer;
		D3D_PRIMITIVE_TOPOLOGY		zTopology;

		//Texturing
		float						zTextureScale;
		unsigned int				zNrOfTextures;
		unsigned int				zTextureCapacity;
		TextureResource**			zTextureResources;
		//bool						zTextureResourceHasChanged[8];	 //**
		string*						zTextureResourceToLoadFileName;
		unsigned int				zNrOfBlendMaps;
		//No need for a capacity variable, we can use zTextureCapacity.
		BlendMap**					zBlendMaps;

		//Collision
		bool						zIsCulled;
		bool						zIsShadowCulled;
		bool						zRecreateBoundingSphere;
		BoundingSphere				zBoundingSphere;

		//Editor
		bool						zUseAIMap;
		unsigned int				zNrOfAINodesPerSide;
		void*						zAIData;
		bool						zAIGridHasChanged;	
		ID3D11ShaderResourceView*	zAIGridShaderResourceView; //Is unique for every terrain so the use of resource is unnecessary.
		float						zAIGridThickness;
		

	private:
		void CreateMesh();
		//Calculates normals for each vertex in the height map
		void CalculateNormals();


	public:
		Terrain();
		Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size);
		virtual ~Terrain();

		//GET-functions
		//Object data
		unsigned int GetSize() const { return this->zSize; }
		D3DXVECTOR3& GetScaleD3DX() { return this->zScale; }
		D3DXMATRIX& GetWorldMatrix() { return this->zWorldMatrix; }

		//Vertex data
		bool HasHeightMapChanged() const { return this->zHeightMapHasChanged; }
		bool HaveNormalsChanged() const { return this->zNormalsHaveChanged; }
		Vertex* GetVerticesPointer() const { return this->zVertices; }
		Buffer* GetVertexBufferPointer() const { return this->zVertexBuffer; }
		int GetNrOfIndices() const { return this->zNrOfIndices; }
		int* GetIndicesPointer() const { return this->zIndices; }
		Buffer* GetIndexBufferPointer() const { return this->zIndexBuffer; }
		Material* GetMaterial() const { return this->zMaterial; }
		D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }
		// Easier access to normals
		inline D3DXVECTOR3& GetNormalAt( unsigned int x, unsigned int y ) const { return zVertices[y * zSize + x].normal; }

		//Textures
		float GetTextureScale() const { return this->zTextureScale; }
		unsigned int GetNrOfTextures() const { return this->zNrOfTextures; }
		TextureResource* GetTexture(unsigned int index) const { return this->zTextureResources[index]; }
		inline string& GetTextureResourceToLoadFileName(unsigned int index) const { return this->zTextureResourceToLoadFileName[index]; }
		int GetNrOfBlendMaps() const { return this->zNrOfBlendMaps; }
		BlendMap* GetBlendMapPointer(unsigned int index) { return this->zBlendMaps[index]; }

		//Collision
		bool IsCulled() const { return this->zIsCulled; }
		bool IsShadowCulled() const { return this->zIsShadowCulled; }
		
		//Editor
		unsigned int GetNrOfAINodesPerSide() const { return this->zNrOfAINodesPerSide; }
		const void* GetAIData() const { return (const void*)this->zAIData; }
		bool HasAIGridChanged() const { return this->zAIGridHasChanged; }
		ID3D11ShaderResourceView*& GetAIShaderResourceView() { return this->zAIGridShaderResourceView; }

		//SET-functions.
		//Object data
		void SetScale(D3DXVECTOR3 scale) { this->zScale = scale; }

		//Vertex data
		void HeightMapHasChanged(bool flag) { this->zHeightMapHasChanged = flag; }
		void NormalsHaveChanged(bool flag) { this->zNormalsHaveChanged = flag; }
		void SetVertexBuffer(Buffer* vertexBuffer);
		void SetIndexBuffer(Buffer* indexBuffer);

		//Textures
		void SetTexture(unsigned int index, TextureResource* textureResource) const { this->zTextureResources[index] = textureResource; }
		void SetTextureResourceToLoadFileName(unsigned int index, string fileName) { this->zTextureResourceToLoadFileName[index] = ""; }
		
		//Collision
		void SetCulled(bool flag) { this->zIsCulled = flag; }
		void SetShadowCulled(bool flag) { this->zIsShadowCulled = flag; }
		BoundingSphere& GetBoundingSphere() { if(this->zRecreateBoundingSphere) { RecreateBoundingSphere(); } return this->zBoundingSphere; }

		//Editor
		bool UseAIMap() { return this->zUseAIMap; }
		void SetAIGridShaderResourceView(ID3D11ShaderResourceView* AIGridShaderResourceView) { this->zAIGridShaderResourceView = AIGridShaderResourceView; }
		void AIGridHasChanged(bool has) {this->zAIGridHasChanged = has; }
		float GetAINodeSize() const { return (float)this->zSize / (float)this->zNrOfAINodesPerSide; }
		float GetAIGridThickness() const { return this->zAIGridThickness; }

		//OTHER.
		//Is used internally when needed, but can be used from the outside for debugging.
		void RecreateWorldMatrix();
		void RecreateBoundingSphere();



		//** iTerrain interface functions ** - for descriptions, see iTerrain.h.
		//GET-functions.
		//Object data
		virtual Vector3 GetScale() const { return Vector3(this->zScale.x, this->zScale.y, this->zScale.z); }
		virtual Vector3 GetPosition() const { return Vector3(this->zPos.x, this->zPos.y, this->zPos.z); }
		
		//Vertex data
		virtual int GetNrOfVertices() const { return this->zNrOfVertices; }
		virtual Vector3 GetVertexPosition(unsigned int index) const { return Vector3(this->zVertices[index].pos.x, this->zVertices[index].pos.y, this->zVertices[index].pos.z); }
		virtual float GetYPositionAt(float x, float z) const throw(...); 


		//SET-functions
		//Object data
		virtual void SetScale(const Vector3& scale);
		virtual void SetDiffuseColor(const Vector3& color);
		
		//Vertex data
		virtual void SetHeightMap(float const* const data); 
		virtual void SetNormals(float const* const data);
		
		//Texture
		virtual void SetTextureScale(float textureScale) { this->zTextureScale = textureScale; }
		virtual void SetTextures(char const* const* const fileNames);
		virtual void SetBlendMaps(unsigned int nrOfBlendMaps, unsigned int* sizes, float const* const* const data);
		
		//Editor
		virtual void UseAIMap(bool use) { this->zUseAIMap = use; }
		virtual void SetAIGrid(unsigned int nrOfAINodesPerSide, void* aiData) { this->zNrOfAINodesPerSide = nrOfAINodesPerSide; this->zAIData = aiData; this->zAIGridHasChanged = true; }
		virtual void SetAIGridThickness(float thickness = 0.001f) { this->zAIGridThickness = thickness; }

		// MaloW ToDO
		// virtual void SetAIMap(int size, float const* data);
		// Do just like blendmap, maybe even use the struct blendmap, and send it to GPU just like it
		// And then render it. think about performance tho, this shouldnt affect client-side.


};

#endif