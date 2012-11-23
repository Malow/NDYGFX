#pragma once

#include "DirectX.h"
#include "iImage.h"
/*
Image class that represents a 2d object on the screen

*/

class Image : public iImage
{
private:
	D3DXVECTOR2 position;
	D3DXVECTOR2 dimensions;
	float opacity;
	ID3D11ShaderResourceView* texture;


public:
	Image(D3DXVECTOR2 pos, D3DXVECTOR2 dim);
	virtual ~Image();

	virtual Vector2 GetPosition() const;
	virtual void SetPosition(Vector2 pos);
	ID3D11ShaderResourceView* GetTexture() const { return this->texture; }
	void SetTexture(ID3D11ShaderResourceView* text) { this->texture = text; }
	virtual Vector2 GetDimensions() const;
	virtual void SetDimensions(Vector2 dims);
	virtual float GetOpacity() const { return this->opacity; }
	virtual void SetOpacity(float opacity) { this->opacity = opacity; }
};