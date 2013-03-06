#pragma once

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"
#include <vector>

class OtherTest
{
private:
	iFBXMesh* copyrightedTempGuy;
	iFBXMesh* soldier;

	iTerrain* T;
	unsigned int vertexSizes;
	std::vector<float> heightData;
	std::vector<Vector3> normalsData;

public:
	OtherTest() {};
	virtual ~OtherTest() {};

	void PreTest();
	void RunTest(float diff);
	void PostTest();
};

void OtherTest::PreTest()
{
	copyrightedTempGuy = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", Vector3(50.0f, 0.0f, 0.0f));	// copyrightedTempGuy = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", Vector3(0.0f, 0.0f, 0.0f));
	copyrightedTempGuy->SetScale(0.2f);

	soldier = GetGraphics()->CreateFBXMesh("Media/Soldier_animated_jump.fbx", Vector3(30.0f, 0.0f, -4.0f));
	soldier->SetScale(0.05f);

	vertexSizes = 2;
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


	// Disable Sun
	//GetGraphics()->SetSunLightDisabled();

	// Create Terrain
	T = GetGraphics()->CreateTerrain(Vector3(0.0f, 0.0f, 0.0f), Vector3(32.0f, 1.0f, 32.0f), vertexSizes);

	// Load Height
	T->SetHeightMap(&heightData[0]);

	// Load Normals
	T->SetNormals(&normalsData[0][0]);
}

void OtherTest::RunTest(float diff)
{
	if ( copyrightedTempGuy )
	{
		if ( GetGraphics()->GetKeyListener()->IsPressed('1') )
		{
			copyrightedTempGuy->SetAnimation("idle_01_dance");
		}
		else if ( GetGraphics()->GetKeyListener()->IsPressed('2') )
		{
			copyrightedTempGuy->SetAnimation(1);
		}
		else if ( GetGraphics()->GetKeyListener()->IsPressed('3') )
		{
			copyrightedTempGuy->SetAnimation(2);
		}
		else if ( GetGraphics()->GetKeyListener()->IsPressed('4') )
		{
			copyrightedTempGuy->SetAnimation(3);
		}
		else if ( GetGraphics()->GetKeyListener()->IsPressed('5') )
		{
			copyrightedTempGuy->SetAnimation(4);
		}
		else if ( GetGraphics()->GetKeyListener()->IsPressed('8') )
		{
			copyrightedTempGuy->BindMesh("_RightHand", soldier);
		}
	}

	if ( GetGraphics()->GetKeyListener()->IsPressed('R') )
	{
		GetGraphics()->ReloadShaders(1);
	}
}
void OtherTest::PostTest()
{
	GetGraphics()->DeleteMesh(copyrightedTempGuy);
	GetGraphics()->DeleteMesh(soldier);

	GetGraphics()->DeleteTerrain(T);
}