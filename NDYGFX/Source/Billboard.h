//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Texture-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h.
//--------------------------------------------------------------------------------------------------
/*
#pragma once

#include "DirectX.h"
#include "iBillboard.h"
#include "ResourceManager.h"


class Billboard : public iBillboard
{
	private:
		D3DXVECTOR3			zPosition;
		D3DXVECTOR2			zSize;
		TextureResource*	zTextureResource;

	public:
		Billboard(D3DXVECTOR3 position, D3DXVECTOR2 size);
		virtual ~Billboard();

		TextureResource* GetTextureResource() const { return this->zTextureResource; }
		void SetTextureResource(TextureResource* textureResource) { this->zTextureResource = textureResource; }

		//** iBillboard interface functions **
		virtual Vector3 GetPosition() const;
		virtual Vector2 GetSize() const;
		virtual void SetPosition(Vector3 position);
		virtual void SetSize(Vector2 size);

};
		*/