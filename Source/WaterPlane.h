#pragma once

#include "Mesh.h"
#include "iWaterPlane.h"

class WaterPlane : public Mesh, public virtual iWaterPlane
{
public:
	WaterPlane(D3DXVECTOR3 pos);
	virtual ~WaterPlane();
};