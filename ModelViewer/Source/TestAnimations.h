#pragma once

#include "TestCase.h"
#include "Graphics.h"
#include <vector>


class TestAnimations : public TestCase
{
	iMesh* mesh;

public:
	TestAnimations()
	{
		
	}
	
	virtual ~TestAnimations()
	{

	}

	virtual void PreTest()
	{
		// Disable Sun
		GetGraphics()->SetSunLightDisabled();

		mesh = GetGraphics()->CreateMesh("Media/Soldier_animated_jump.fbx", Vector3(0.0f, 0.0f, 0.0f));
		mesh->SetScale(0.1f);
	}

	virtual void PostTest()
	{
		GetGraphics()->DeleteMesh(mesh);
	}
};