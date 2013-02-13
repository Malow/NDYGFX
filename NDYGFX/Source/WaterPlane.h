#pragma once

#include "Mesh.h"
#include "iWaterPlane.h"
#include "Buffer.h"

class WaterPlane : public Mesh, public virtual iWaterPlane
{
private:
	TextureResource* textureResource;
	TextureResource* textureResource2;
	Vertex* verts;
	int nrOfVerts;
	Buffer* vertBuffer;

public:
	WaterPlane(D3DXVECTOR3 pos);
	virtual ~WaterPlane();

	void SetTexture(TextureResource* text) { this->textureResource = text; }
	void SetTexture2(TextureResource* text) { this->textureResource2 = text; }
	TextureResource* GetTextureResource() const { return this->textureResource; }
	TextureResource* GetTextureResource2() const { return this->textureResource2; }
	virtual MaloW::Array<MeshStrip*>* GetStrips();
	Vertex* GetVerts() const { return this->verts; }
	int GetNrOfVerts() const { return this->nrOfVerts; }
	void SetVertexBuffer(Buffer* vertBuffer) { this->vertBuffer = vertBuffer; }
	Buffer* GetVertexBuffer() const { return this->vertBuffer; }
};