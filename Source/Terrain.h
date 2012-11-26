#ifndef TERRAIN_H
#define TERRAIN_H

#include "Vertex.h"
#include "StaticMesh.h"
#include "iTerrain.h"
#include <Vector.h>

class Terrain : public virtual StaticMesh, public virtual iTerrain
{
private:
	int SIZE;
	D3DXVECTOR3 dimensions;

public:

	Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 dimension, string texture, string heightmap, int vertexSize);
	virtual ~Terrain();

	virtual bool LoadAndApplyHeightMap(const char* fileName);
	virtual void filter(unsigned int smootheness);
	virtual void calculateNormals();
	virtual float getYPositionAt(float x, float z);
};

#endif