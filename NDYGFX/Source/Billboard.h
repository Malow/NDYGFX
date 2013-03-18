//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//--------------------------------------------------------------------------------------------------

#pragma once

#include "DirectX.h"
#include "iBillboard.h"
#include "ResourceManager.h"
#include "Vertex.h"


class Billboard : public iBillboard
{
	private:
		VertexBillboard1	zVertex; //position, size(width, height), color
		TextureResource*	zTextureResource;
		bool				zRenderShadowFlag;
		bool				zIsCameraCulled;
		bool				zIsShadowCulled;
			
	public:
		Billboard();
		Billboard(const VertexBillboard1& vertex, bool renderShadowFlag = true);
		Billboard(const D3DXVECTOR3& position, const D3DXVECTOR2& size, const D3DXVECTOR3& color, bool renderShadowFlag = true);
		virtual ~Billboard();


		D3DXVECTOR3 GetPositionD3DX() const { return D3DXVECTOR3(this->zVertex.posAndSizeX.x, this->zVertex.posAndSizeX.y, this->zVertex.posAndSizeX.z); }
		D3DXVECTOR2 GetSizeD3DX() const { return D3DXVECTOR2(this->zVertex.posAndSizeX.w, this->zVertex.sizeYAndColor.x); }
		D3DXVECTOR3 GetColorD3DX() const { return D3DXVECTOR3(this->zVertex.sizeYAndColor.y, this->zVertex.sizeYAndColor.z, this->zVertex.sizeYAndColor.w); }
		const VertexBillboard1& GetVertex() const { return this->zVertex; }
		TextureResource* GetTextureResource() const { return this->zTextureResource; }
		bool GetRenderShadowFlag() const { return this->zRenderShadowFlag; }
		bool IsCameraCulled() { return this->zIsCameraCulled; }
		bool IsShadowCulled() { return this->zIsShadowCulled; }

		void SetPosition(D3DXVECTOR3& position);
		void SetSize(D3DXVECTOR2& size);
		void SetColor(D3DXVECTOR3& color);
		void SetTextureResource(TextureResource* textureResource) { this->zTextureResource = textureResource; }
		//void SetRenderShadowFlag(bool flag) { this->zRenderShadowFlag = flag; }
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
		virtual void SetColor(Vector3 color);*/

		virtual void SetRenderShadowFlag(bool flag) { this->zRenderShadowFlag = flag; }


};