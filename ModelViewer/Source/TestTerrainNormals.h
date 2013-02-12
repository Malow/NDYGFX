#pragma once

#include "TestCase.h"
#include "Graphics.h"
#include <vector>


class TestTerrainNormals : public TestCase
{
	iTerrain* T;
	unsigned int vertexSizes;
	std::vector<float> heightData;
	std::vector<Vector3> normalsData;

public:
	TestTerrainNormals() :
		vertexSizes(2)
	{
		heightData.resize(vertexSizes*vertexSizes);
		normalsData.resize(vertexSizes*vertexSizes);

		for( unsigned int x=0; x<vertexSizes*vertexSizes; ++x )
		{
			heightData[x] = 0.0f;
		}

		for( unsigned int x=0; x<vertexSizes*vertexSizes; ++x )
		{
			normalsData[x] = Vector3(1.0f, 1.0f, 1.0f);
		}

		//normalsData[0] = Vector3(1.0f, 0.0f, 0.0f);
		//normalsData[3] = Vector3(0.0f, 0.0f, 1.0f);
	}
	
	virtual ~TestTerrainNormals()
	{
	}

	virtual void PreTest()
	{
		// Disable Sun
		GetGraphics()->SetSunLightDisabled();
		
		// Create Terrain
		T = GetGraphics()->CreateTerrain(Vector3(0.0f, 0.0f, 0.0f), Vector3(32.0f, 1.0f, 32.0f), vertexSizes);

		// Load Height
		T->SetHeightMap(&heightData[0]);

		// Load Normals
		T->SetNormals(&normalsData[0][0]);
	}

	virtual void PostTest()
	{
		GetGraphics()->DeleteTerrain(T);
	}
};