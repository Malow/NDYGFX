#pragma once

#include "DirectX.h"

class BoundingSphere
{
public:
	D3DXVECTOR3 center;
	float radius;

	BoundingSphere()
	{
		this->center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		this->radius = 0.0f;
	}

	BoundingSphere(const D3DXVECTOR3& minCorner, const D3DXVECTOR3& maxCorner)
	{
		this->center = (maxCorner + minCorner) / 2.0f;

		D3DXVECTOR3 boxLength = D3DXVECTOR3(maxCorner - minCorner) / 2.0f;
		this->radius = D3DXVec3Length(&boxLength);
	}

	virtual ~BoundingSphere() {}
};