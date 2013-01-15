#ifndef OBJECT_3D
#define OBJECT_3D

#include "DirectX.h"
#include "Buffer.h"
#include "Vertex.h"
#include "TextureResource.h"
#include "ResourceManager.h"

class Object3D
{
private:
	Buffer* verts;
	Buffer* inds;
	TextureResource* textureResource; //**TILLMAN
	//ID3D11ShaderResourceView* texture; //**TILLMAN
	D3D_PRIMITIVE_TOPOLOGY topology;

public:
	Object3D(Buffer* verts, Buffer* inds, TextureResource* textureResource, D3D_PRIMITIVE_TOPOLOGY topology);
	virtual ~Object3D();

	Buffer* GetVertBuff() { return this->verts; }
	Buffer* GetIndsBuff() { return this->inds; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() { return this->topology; }
	TextureResource* GetTextureResource() { return this->textureResource; }

};

#endif