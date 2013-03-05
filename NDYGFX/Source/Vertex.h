#ifndef VERTEX_H
#define VERTEX_H

#include "DirectX.h"

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 texCoord;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 color;

	Vertex(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm, D3DXVECTOR3 _col)
	{
		pos = _pos;
		texCoord = _texCoord;
		normal = _norm;
		color = _col;
	}
	Vertex(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm)
	{
		pos = _pos;
		texCoord = _texCoord;
		normal = _norm;
		color = D3DXVECTOR3(0, 0, 0);
	}
	Vertex(D3DXVECTOR3 _pos, D3DXVECTOR3 _norm, D3DXVECTOR3 _color)
	{
		pos = _pos;
		texCoord = D3DXVECTOR2(0, 0);
		normal = _norm;
		color = _color;
	}
	Vertex()
	{
		pos = D3DXVECTOR3(0, 0, 0);
		texCoord = D3DXVECTOR2(0, 0);
		normal = D3DXVECTOR3(0, 0, 0);
		color = D3DXVECTOR3(0, 0, 0);
	}
	Vertex(const Vertex* origObj)
	{
		pos = origObj->pos;
		texCoord = origObj->texCoord;
		normal = origObj->normal;
		color = origObj->color;
	}
};



struct VertexNormalMap : public Vertex
{
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;

	VertexNormalMap(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm, D3DXVECTOR3 _tang, D3DXVECTOR3 _binorm) : Vertex(_pos, _texCoord, _norm)
	{
		tangent = _tang;
		binormal = _binorm;
	}
	VertexNormalMap(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm) : Vertex(_pos, _texCoord, _norm)
	{
		tangent = D3DXVECTOR3(0, 0, 0);
		binormal = D3DXVECTOR3(0, 0, 0);
	}
	VertexNormalMap() : Vertex()
	{
		tangent = D3DXVECTOR3(0, 0, 0);
		binormal = D3DXVECTOR3(0, 0, 0);
	}
	VertexNormalMap(const VertexNormalMap* origObj)
	{
		pos = origObj->pos;
		texCoord = origObj->texCoord;
		normal = origObj->normal;
		tangent = origObj->tangent;
		binormal = origObj->binormal;
	}
};

struct VertexNormalMapCompressed1
{
	D3DXVECTOR4 pos_TexU;
	D3DXVECTOR4 texV_Norm;
	D3DXVECTOR3 tangent;	
	D3DXVECTOR3 binormal;

	VertexNormalMapCompressed1(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm, D3DXVECTOR3 _tang, D3DXVECTOR3 _binorm)
	{
		pos_TexU = D3DXVECTOR4(_pos, _texCoord.x);
		texV_Norm = D3DXVECTOR4(_texCoord.y, _norm.x, _norm.y, _norm.z);
		tangent = _tang;
		binormal = _binorm;
	}

	VertexNormalMapCompressed1(const VertexNormalMapCompressed1* origObj)
	{
		pos_TexU = origObj->pos_TexU;
		texV_Norm = origObj->texV_Norm;
		tangent = origObj->tangent;
		binormal = origObj->binormal;
	}
	inline D3DXVECTOR2 GetTexCoords() { return D3DXVECTOR2(pos_TexU.w, texV_Norm.x); }
	inline D3DXVECTOR3 GetPos() { return D3DXVECTOR3(pos_TexU.x, pos_TexU.y, pos_TexU.z); }
	inline D3DXVECTOR3 GetNormal() { return D3DXVECTOR3(texV_Norm.y, texV_Norm.z, texV_Norm.w); }
};




struct ParticleVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 dimensions;
	float opacity;
	
	ParticleVertex(D3DXVECTOR3 p, D3DXVECTOR2 d, float op)
	{
		pos = p;
		dimensions = d;
		opacity = op;
	}

	ParticleVertex()
	{
		pos = D3DXVECTOR3(0, 0, 0);
		dimensions = D3DXVECTOR2(0, 0);
		opacity = 0.0f;
	}
};

#endif