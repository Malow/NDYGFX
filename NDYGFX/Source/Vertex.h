#ifndef VERTEX_H
#define VERTEX_H

#include "DirectX.h"


struct VertexBillboard1
{
	D3DXVECTOR4 posAndSizeX;
	D3DXVECTOR4 sizeYAndColor;

	VertexBillboard1(const D3DXVECTOR3& _pos, const D3DXVECTOR2& _size, const D3DXVECTOR3& _col)
		: posAndSizeX(_pos.x, _pos.y, _pos.z, _size.x), sizeYAndColor(_size.y, _col.x, _col.y, _col.z)
	{

	}
	VertexBillboard1(const D3DXVECTOR4& _posAndSizeX, const D3DXVECTOR4& _sizeYAndColor)
		: posAndSizeX(_posAndSizeX), sizeYAndColor(_sizeYAndColor)
	{

	}
	VertexBillboard1()
		: posAndSizeX(0.0f, 0.0f, 0.0f, 0.0f), sizeYAndColor(0.0f, 0.0f, 0.0f, 0.0f)
	{

	}
	VertexBillboard1(const VertexBillboard1& origObj)
		: posAndSizeX(origObj.posAndSizeX), sizeYAndColor(origObj.sizeYAndColor)
	{

	}

	D3DXVECTOR3 GetPosition() const { return D3DXVECTOR3(posAndSizeX.x, posAndSizeX.y, posAndSizeX.z); }
	D3DXVECTOR3 GetColor() const { return D3DXVECTOR3(sizeYAndColor.y, sizeYAndColor.z, sizeYAndColor.w); }
};
/*
struct VertexBillboardCompressed1
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 texAndSize;
	D3DXVECTOR3 color;
	
	VertexBillboardCompressed1(const D3DXVECTOR3& _pos, const D3DXVECTOR2& _tex, const D3DXVECTOR2& _size, const D3DXVECTOR3& _col)
		: pos(_pos), texAndSize(_tex.x, _tex.y, _size.x, _size.y), color(_col)
	{
		
	}
	VertexBillboardCompressed1(const D3DXVECTOR3& _pos, const D3DXVECTOR4& _texAndSize, const D3DXVECTOR3& _col)
		: pos(_pos), texAndSize(_texAndSize), color(_col)
	{
		
	}
	VertexBillboardCompressed1()
		: pos(0.0f, 0.0f, 0.0f), texAndSize(0.0f, 0.0f, 0.0f, 0.0f), color(0.0f, 0.0f, 0.0f)
	{
		
	}
	VertexBillboardCompressed1(const VertexBillboardCompressed1& origObj)
		: pos(origObj.pos), texAndSize(origObj.texAndSize), color(origObj.color)
	{
		
	}
};*/


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