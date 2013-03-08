//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Texture-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h.
//--------------------------------------------------------------------------------------------------

#pragma once

#include "DirectX.h"
#include "iBillboard.h"
#include "ResourceManager.h"
#include "Vertex.h"


class Billboard : public iBillboard
{
	private:
		/*D3DXVECTOR3			zPosition;
		D3DXVECTOR2			zSize;
		D3DXVECTOR4			zColor;*/
		VertexBillboardCompressed1	zVertex; //position, texture coordinates, size(width, height), color
		TextureResource*			zTextureResource;

	public:
		Billboard();
		Billboard(const VertexBillboardCompressed1& vertex);
		Billboard(const D3DXVECTOR3& position, const D3DXVECTOR2& texCoord, const D3DXVECTOR2& size, const D3DXVECTOR3& color);
		virtual ~Billboard();


		D3DXVECTOR3 GetPositionD3DX() const { return this->zVertex.pos; }
		D3DXVECTOR2 GetTexCoordD3DX() const { return D3DXVECTOR2(this->zVertex.texAndSize.x, this->zVertex.texAndSize.y); }
		D3DXVECTOR2 GetSizeD3DX() const { return D3DXVECTOR2(this->zVertex.texAndSize.z, this->zVertex.texAndSize.w); }
		D3DXVECTOR3 GetColorD3DX() const { return this->zVertex.color; }
		const VertexBillboardCompressed1& GetVertex() const { return this->zVertex; }
		TextureResource* GetTextureResource() const { return this->zTextureResource; }
		void SetPosition(D3DXVECTOR3 position) { this->zVertex.pos = position; }
		void SetSize(D3DXVECTOR2 size) { this->zVertex.texAndSize = D3DXVECTOR4(this->zVertex.texAndSize.x, this->zVertex.texAndSize.y, size.x, size.y); }
		void SetColor(D3DXVECTOR3 color) { this->zVertex.color = color; }
		
		void SetTextureResource(TextureResource* textureResource) { this->zTextureResource = textureResource; }



		//** iBillboard interface functions **
		virtual Vector3 GetPosition() const;
		virtual Vector2 GetTexCoord() const;
		virtual Vector2 GetSize() const;
		virtual Vector3 GetColor() const;
		virtual void SetPosition(Vector3 position);
		virtual void SetTexCoord(Vector2 texCoord);
		virtual void SetSize(Vector2 size);
		virtual void SetColor(Vector3 color);


};