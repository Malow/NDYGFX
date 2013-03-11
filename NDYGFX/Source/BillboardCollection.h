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


class BillboardCollection : public iBillboardCollection
{
	private:
		unsigned int				zNrOfVertices;
		//std::vector<VertexBillboardCompressed1>zVertices; //position(world), texture coordinates, size(width, height), color
		VertexBillboard1*	zVertices; //position(world), texture coordinates, size(width, height), color
		TextureResource*			zTextureResource;
		D3DXVECTOR3					zOffsetVector; //offset added to world positions, can be seen as a "world"-matrix.

	public:
		BillboardCollection();
		//BillboardCollection(const std::vector<VertexBillboardCompressed1>& vertices, const D3DXVECTOR3& offsetVector)
		BillboardCollection(unsigned int nrOfVertices, const VertexBillboard1* vertices, const D3DXVECTOR3& offsetVector);
		BillboardCollection(unsigned int nrOfVertices, const D3DXVECTOR3* positions,
							const D3DXVECTOR2* sizes, const D3DXVECTOR3* colors, const D3DXVECTOR3& offsetVector);
		virtual ~BillboardCollection();

		unsigned int GetNrOfVertices() { return this->zNrOfVertices; }

		//D3DXVECTOR3 GetPositionD3DX(unsigned int vertexIndex)					const { return this->zVertices[vertexIndex].pos; }
		//D3DXVECTOR2 GetTexCoordD3DX(unsigned int vertexIndex)					const { return D3DXVECTOR2(this->zVertices[vertexIndex].texAndSize.x, this->zVertices[vertexIndex].texAndSize.y); }
		//D3DXVECTOR2 GetSizeD3DX(unsigned int vertexIndex)						const { return D3DXVECTOR2(this->zVertices[vertexIndex].texAndSize.z, this->zVertices[vertexIndex].texAndSize.w); }
		//D3DXVECTOR3 GetColorD3DX(unsigned int vertexIndex)						const { return this->zVertices[vertexIndex].color; }
		const VertexBillboard1& GetVertex(unsigned int vertexIndex)	const { return this->zVertices[vertexIndex]; }
		const TextureResource* GetTextureResource()								const { return this->zTextureResource; }
		const D3DXVECTOR3& GetOffsetVector()									const { return this->zOffsetVector; }

		//void SetPosition(unsigned int vertexIndex, D3DXVECTOR3 position){ this->zVertices[vertexIndex].pos = position; }
		//void SetSize(unsigned int vertexIndex, D3DXVECTOR2 size)		{ this->zVertices[vertexIndex].texAndSize = D3DXVECTOR4(this->zVertices[vertexIndex].texAndSize.x, this->zVertices[vertexIndex].texAndSize.y, size.x, size.y); }
		//void SetColor(unsigned int vertexIndex, D3DXVECTOR3 color)		{ this->zVertices[vertexIndex].color = color; }
		//VertexBillboardCompressed1& SetVertex(unsigned int vertexIndex, VertexBillboardCompressed1& vertex)	{ this->zVertices[vertexIndex] = vertex; }
		//void SetTextureResource(TextureResource* textureResource)		{ this->zTextureResource = textureResource; }



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



};