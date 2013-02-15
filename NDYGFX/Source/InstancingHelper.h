#pragma once

//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Helper class for hardware instancing of billboards and meshes.
//
//--------------------------------------------------------------------------------------------------

#include "Vertex.h"
#include "DirectX.h"
#include <vector>

class InstancingHelper
{
private:
	unsigned int instanceCapacityBillboard;
	unsigned int instanceTotalCountBillboard; 
	unsigned int nrOfInstanceGroupsBillboard;

	std::vector<Vertex>			instancesDataBillboard;
	ID3D11ShaderResourceView**	instanceSRVsBillboard;

	std::vector<unsigned int>				instanceGroupCount;
	std::vector<unsigned int>				instanceGroupStartLocation;
	std::vector<ID3D11ShaderResourceView*>	instanceGroupSRVBillboard;


	ID3D11Buffer* instanceBufferBillboard;


public:
};