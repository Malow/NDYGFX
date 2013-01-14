#ifndef OBJECT_3D
#define OBJECT_3D

#include "DirectX.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Texture.h"
#include "ResourceManager.h"

class Object3D
{
private:
	Buffer* verts;
	Buffer* inds;
	Texture* texture; //**TILLMAN
	//ID3D11ShaderResourceView* texture; //**TILLMAN
	D3D_PRIMITIVE_TOPOLOGY topology;

public:
	Object3D(Buffer* verts, Buffer* inds, Texture* texture, D3D_PRIMITIVE_TOPOLOGY topology);
	virtual ~Object3D();

	Buffer* GetVertBuff() { return this->verts; }
	Buffer* GetIndsBuff() { return this->inds; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() { return this->topology; }
	Texture* GetTexture() { return this->texture; }

};

#endif