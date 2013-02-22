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

public:
	Decal(Vector3 pos);
	virtual ~Decal();

	float GetSize() const { return this->size; }
	void SetSize(float size) { this->size = size; }
	Vector3 GetPosition() const { return this->position; }
	void SetPosition(Vector3 pos) { this->position = pos; }
	void SetTexture(TextureResource* text) { this->textureResource = text; }
	TextureResource* GetTextureResource() const { return this->textureResource; }
	D3DXMATRIX GetMatrix() const { return this->matrix; }
	void SetMatrix(D3DXMATRIX mat) { this->matrix = mat; }
};