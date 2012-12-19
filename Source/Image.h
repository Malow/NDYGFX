#pragma once

#include "DirectX.h"
#include "iImage.h"
#include "ResourceManager.h"
/*
Image class that represents a 2d object on the screen

*/

class Image : public iImage
{
private:
	D3DXVECTOR2 position;
	D3DXVECTOR2 dimensions;
	float opacity;
	Texture* texture;


public:
	Image(D3DXVECTOR2 pos, D3DXVECTOR2 dim);
	virtual ~Image();

	virtual Vector2 GetPosition() const;
	virtual void SetPosition(Vector2 pos);
	Texture* GetTexture() const { return this->texture; }
	void SetTexture(Texture* text) { this->texture = text; }
	virtual Vector2 GetDimensions() const;
	virtual void SetDimensions(Vector2 dims);
	virtual float GetOpacity() const { return this->opacity; }
	virtual void SetOpacity(float opacity) { this->opacity = opacity; }
};