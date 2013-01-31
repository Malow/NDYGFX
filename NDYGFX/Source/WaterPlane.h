#pragma once

#include "Mesh.h"
#include "iWaterPlane.h"

class WaterPlane : public Mesh, public virtual iWaterPlane
{
private:
	TextureResource* textureResource;

public:
	WaterPlane(D3DXVECTOR3 pos);
	virtual ~WaterPlane();

	void SetTexture(TextureResource* text) { this->textureResource = text; }
	virtual MaloW::Array<MeshStrip*>* GetStrips();
};