#pragma once

#include "Mesh.h"
#include "iWaterPlane.h"
#include "Buffer.h"

class WaterPlane : public Mesh, public virtual iWaterPlane
{
private:
	TextureResource* textureResource;
	Vertex* verts;
	int nrOfVerts;
	Buffer* vertBuffer;

public:
	WaterPlane(D3DXVECTOR3 pos);
	virtual ~WaterPlane();

	void SetTexture(TextureResource* text) { this->textureResource = text; }
	TextureResource* GetTextureResource() const { return this->textureResource; }
	virtual MaloW::Array<MeshStrip*>* GetStrips();
	Vertex* GetVerts() const { return this->verts; }
	int GetNrOfVerts() const { return this->nrOfVerts; }
	void SetVertexBuffer(Buffer* vertBuffer) { this->vertBuffer = vertBuffer; }
	Buffer* GetVertexBuffer() const { return this->vertBuffer; }
};