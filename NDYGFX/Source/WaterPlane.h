#pragma once

#include "Mesh.h"
#include "iWaterPlane.h"
#include "Buffer.h"

#pragma warning ( push )
#pragma warning ( disable : 4250 ) //  warning C4250: 'WaterPlane' : inherits via dominance

class WaterPlane : public Mesh, public virtual iWaterPlane
{
private:
	TextureResource* textureResource;
	TextureResource* textureResource2;
	Vertex* verts;
	int nrOfVerts;
	Buffer* vertBuffer;

public:
	WaterPlane(D3DXVECTOR3 pos, string filePath);
	virtual ~WaterPlane();

	bool VertexDataHasChanged;

	void SetTexture(TextureResource* text) { this->textureResource = text; }
	void SetTexture2(TextureResource* text) { this->textureResource2 = text; }
	TextureResource* GetTextureResource() const { return this->textureResource; }
	TextureResource* GetTextureResource2() const { return this->textureResource2; }
	virtual MaloW::Array<MeshStrip*>* GetStrips();
	Vertex* GetVerts() const { return this->verts; }
	int GetNrOfVerts() const { return this->nrOfVerts; }
	void SetVertexBuffer(Buffer* vertBuffer) { this->vertBuffer = vertBuffer; }
	Buffer* GetVertexBuffer() const { return this->vertBuffer; }

	virtual void SetVertexPosition(Vector3 pos, int vertexIndex);
};

#pragma warning ( pop )