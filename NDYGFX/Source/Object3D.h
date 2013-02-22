#pragma once

#include "DirectX.h"
#include "Buffer.h"
#include "Vertex.h"
#include "TextureResource.h"
#include "BufferResource.h"
#include "ResourceManager.h"

class Object3D
{
private:
	BufferResource* verts;
	BufferResource* inds;
	TextureResource* textureResource;
	TextureResource* normalMap;
	D3D_PRIMITIVE_TOPOLOGY topology;

public:
	Object3D(BufferResource* verts, BufferResource* inds, TextureResource* textureResource, 
		TextureResource* normalMap, D3D_PRIMITIVE_TOPOLOGY topology);
	virtual ~Object3D();

	BufferResource* GetVertexBufferResource() { return this->verts; }
	BufferResource* GetIndexBufferResource() { return this->inds; }
	/* DEPRECATED, please use GetVertexBufferResource. */
	Buffer* GetVertBuff();
	/* DEPRECATED, please use GetIndexBufferResource. */
	Buffer* GetIndsBuff();
	D3D_PRIMITIVE_TOPOLOGY GetTopology() { return this->topology; }
	TextureResource* GetTextureResource() { return this->textureResource; }
	TextureResource* GetNormalMapResource() { return this->normalMap; }

};