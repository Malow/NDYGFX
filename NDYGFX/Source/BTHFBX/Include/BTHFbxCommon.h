#ifndef __bthfbx_common_h__
#define __bthfbx_common_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include <string>
#include <cmath>

#define BTHFBX_MAXBONES_PER_VERTEX 4
#define BTHFBX_MAXBONES_PER_MESH   100

struct BTHFBX_MATRIX
{
	union
	{
		float f[16];
		float m[4][4];
	};
};

struct BTHFBX_VEC4
{
	float x, y, z, w;

	BTHFBX_VEC4()
	{
		x = y = z = w = 0.0f;
	}

	BTHFBX_VEC4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

struct BTHFBX_VEC3
{
	float x, y, z;

	BTHFBX_VEC3()
	{
		x = y = z = 0.0f;
	}

	BTHFBX_VEC3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct BTHFBX_VEC2
{
	float x, y;

	BTHFBX_VEC2()
	{
		x = y = 0.0f;
	}

	BTHFBX_VEC2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};

struct BTHFBX_BLEND_WEIGHT_DATA
{
	//unsigned int NumWeights;
	float BoneIndices[4];
	float BlendWeights[4];
};

struct BTHFBX_AABB_DATA
{
	BTHFBX_VEC3	Min;
	BTHFBX_VEC3 Max;
};

struct BTHFBX_OBB_DATA
{
	BTHFBX_VEC3 Corners[8];
};

struct BTHFBX_RAY
{
	BTHFBX_VEC3	Origin;
	BTHFBX_VEC3 Direction;
};

struct BTHFBX_RAY_BOX_RESULT
{
	float DistanceToHit;
	int	JointIndex;
};

#endif