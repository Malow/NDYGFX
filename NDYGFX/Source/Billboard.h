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


class Billboard : public iBillboard
{
	private:
		D3DXVECTOR3			zPosition;
		D3DXVECTOR2			zSize;
		D3DXVECTOR4			zColor;
		TextureResource*	zTextureResource;

	public:
		Billboard();
		Billboard(D3DXVECTOR3 position, D3DXVECTOR2 size, D3DXVECTOR4 color = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
		virtual ~Billboard();


		D3DXVECTOR3 GetPositionD3DX() const { return this->zPosition; }
		D3DXVECTOR2 GetSizeD3DX() const { return this->zSize; }
		TextureResource* GetTextureResource() const { return this->zTextureResource; }
		D3DXVECTOR4 GetColorD3DX() const { return this->zColor; }
		void SetPosition(D3DXVECTOR3 position) { this->zPosition = position; }
		void SetSize(D3DXVECTOR2 size) { this->zSize = size; }
		void SetColor(D3DXVECTOR4 color) { this->zColor = color; }
		void SetTextureResource(TextureResource* textureResource) { this->zTextureResource = textureResource; }



		//** iBillboard interface functions **
		virtual Vector3 GetPosition() const;
		virtual Vector2 GetSize() const;
		virtual Vector4 GetColor() const;
		virtual void SetPosition(Vector3 position);
		virtual void SetSize(Vector2 size);
		virtual void SetColor(Vector4 color);


};