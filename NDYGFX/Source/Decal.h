#pragma once

#include "iDecal.h"
#include "DirectX.h"
#include "ResourceManager.h"

class Decal : public virtual iDecal
{
private:
	D3DXMATRIX matrix;
	Vector3 position;
	float size;
	TextureResource* textureResource;

	MeshStrip* strip;

public:
	Decal(Vector3 pos, int latitude, int longitude);
	virtual ~Decal();

	float GetSize() const { return this->size; }
	void SetSize(float size) { this->size = size; }
	Vector3 GetPosition() const { return this->position; }
	void SetPosition(Vector3 pos) { this->position = pos; }
	void SetTexture(TextureResource* text) { this->textureResource = text; }
	TextureResource* GetTextureResource() const { return this->textureResource; }
	D3DXMATRIX GetMatrix() const;
	D3DXMATRIX GetWorldMatrix();
	void SetMatrix(D3DXMATRIX mat) { this->matrix = mat; }

	MeshStrip* GetStrip() { return this->strip; }
};