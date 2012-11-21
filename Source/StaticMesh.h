#pragma once

#include "DirectX.h"
#include "Mesh.h"


class StaticMesh : public Mesh
{
public:
	StaticMesh(D3DXVECTOR3 pos);
	virtual ~StaticMesh();

	void LoadFromFile(string file);
	virtual MaloW::Array<MeshStrip*>* GetStrips();
};