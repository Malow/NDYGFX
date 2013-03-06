#pragma once

#include "DirectX.h"

#pragma warning ( push )
#pragma warning ( disable : 4238 ) // warning C4238: nonstandard extension used : class rvalue used as lvalue

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

	BoundingSphere(D3DXVECTOR3 minCorner, D3DXVECTOR3 maxCorner)
	{
		this->center = (maxCorner + minCorner) / 2.0f;
		this->radius = D3DXVec3Length(&((maxCorner - minCorner) / 2.0f));
	}

	virtual ~BoundingSphere() {}
};

#pragma warning ( pop )