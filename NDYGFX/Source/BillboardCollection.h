//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
//
//--------------------------------------------------------------------------------------------------

#pragma once

#include "DirectX.h"
#include "iBillboardCollection.h"
#include "ResourceManager.h"
#include "Vertex.h"
#include "Vector.h"

struct BillboardData
{
	VertexBillboard1			s_Vertex;
	ID3D11ShaderResourceView*	s_SRV; 

	BillboardData() : s_Vertex(), s_SRV(NULL) {}
	BillboardData(VertexBillboard1 vertex, ID3D11ShaderResourceView* srv) : s_Vertex(vertex), s_SRV(srv) {}
};

class BillboardCollection : public iBillboardCollection
{
	private:
		unsigned int		zNrOfVertices;
		VertexBillboard1*	zVertices; //position(world), texture coordinates, size(width, height), color
		TextureResource*	zTextureResource;
		D3DXVECTOR3			zOffsetVector; //offset added to world positions, can be seen as a "world"-matrix.
		bool				zRenderShadowFlag;
		float				zCullNearDistance;
		float				zCullFarDistance;
		bool				zIsCameraCulled;
		bool				zIsShadowCulled;
		D3DXVECTOR3			zMinPos;
		D3DXVECTOR3			zMaxPos;

		std::vector<BillboardData>	zBillboardData;//For instancing

	private:
		bool pVerticesChanged;
		void RecalculateMinAndMaxPos();

	public:
		BillboardCollection();
		BillboardCollection(unsigned int nrOfVertices, const VertexBillboard1* vertices, const D3DXVECTOR3& offsetVector,
							float cullNearDistance = 0.0f, float cullFarDistance = std::numeric_limits<float>::infinity());
		BillboardCollection(unsigned int nrOfVertices, const D3DXVECTOR3* positions,
							const D3DXVECTOR2* sizes, const D3DXVECTOR3* colors, const D3DXVECTOR3& offsetVector,
							float cullNearDistance = 0.0f, float cullFarDistance = std::numeric_limits<float>::infinity());
		virtual ~BillboardCollection();

		inline const std::vector<BillboardData>& GetBillboardData() const { return this->zBillboardData; }
		inline const VertexBillboard1* GetVertices() const { return zVertices; }
		unsigned int GetNrOfVertices() { return this->zNrOfVertices; }
		//D3DXVECTOR3 GetPositionD3DX(unsigned int vertexIndex)					const { return this->zVertices[vertexIndex].pos; }
		//D3DXVECTOR2 GetTexCoordD3DX(unsigned int vertexIndex)					const { return D3DXVECTOR2(this->zVertices[vertexIndex].texAndSize.x, this->zVertices[vertexIndex].texAndSize.y); }
		//D3DXVECTOR2 GetSizeD3DX(unsigned int vertexIndex)						const { return D3DXVECTOR2(this->zVertices[vertexIndex].texAndSize.z, this->zVertices[vertexIndex].texAndSize.w); }
		//D3DXVECTOR3 GetColorD3DX(unsigned int vertexIndex)						const { return this->zVertices[vertexIndex].color; }
		inline const VertexBillboard1& GetVertex(unsigned int vertexIndex)	const { return this->zVertices[vertexIndex]; }
		const TextureResource* GetTextureResource()	const { return this->zTextureResource; }
		const D3DXVECTOR3& GetOffsetVector() const { return this->zOffsetVector; }
		bool GetRenderShadowFlag() const { return this->zRenderShadowFlag; }
		bool IsCameraCulled() { return this->zIsCameraCulled; }
		bool IsShadowCulled() { return this->zIsShadowCulled; }
		const D3DXVECTOR3& GetMinPos();
		const D3DXVECTOR3& GetMaxPos();
		D3DXVECTOR3 CalcWorldPos();

		//void SetPosition(unsigned int vertexIndex, D3DXVECTOR3 position){ this->zVertices[vertexIndex].pos = position; }
		//void SetSize(unsigned int vertexIndex, D3DXVECTOR2 size)		{ this->zVertices[vertexIndex].texAndSize = D3DXVECTOR4(this->zVertices[vertexIndex].texAndSize.x, this->zVertices[vertexIndex].texAndSize.y, size.x, size.y); }
		//void SetColor(unsigned int vertexIndex, D3DXVECTOR3 color)		{ this->zVertices[vertexIndex].color = color; }
		//VertexBillboardCompressed1& SetVertex(unsigned int vertexIndex, VertexBillboardCompressed1& vertex)	{ this->zVertices[vertexIndex] = vertex; }
		void SetTextureResource(TextureResource* textureResource);
		void SetIsCameraCulledFlag(bool flag) { this->zIsCameraCulled = flag; }
		void SetIsShadowCulledFlag(bool flag) { this->zIsShadowCulled = flag; }


		//** iBillboard interface functions **
		/*virtual Vector3 GetPosition() const;
		virtual Vector2 GetTexCoord() const;
		virtual Vector2 GetSize() const;
		virtual Vector3 GetColor() const;
		virtual void SetPosition(Vector3 position);
		virtual void SetTexCoord(Vector2 texCoord);
		virtual void SetSize(Vector2 size);
		virtual void SetColor(Vector3 color);
		*/

		virtual float GetCullNearDistance() const { return this->zCullNearDistance; }
		virtual float GetCullFarDistance() const { return this->zCullFarDistance; }

		virtual void SetRenderShadowFlag(bool flag) { this->zRenderShadowFlag = flag; }
		virtual void SetCullNearDistance(float distance) { this->zCullNearDistance = distance; }
		virtual void SetCullFarDistance(float distance) { this->zCullFarDistance = distance; }


};