#pragma once

#include "TestCase.h"
#include "Graphics.h"
#include <vector>


class TestAnimations : public TestCase
{
	iFBXMesh* copyrightedTempGuy;
	iFBXMesh* soldier;

public:
	TestAnimations() :
		copyrightedTempGuy(0),
		soldier(0)
	{
		
	}
	
	virtual ~TestAnimations()
	{

	}

	virtual void PreTest()
	{
		copyrightedTempGuy = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", Vector3(0.0f, 0.0f, 0.0f));	// copyrightedTempGuy = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", Vector3(0.0f, 0.0f, 0.0f));

		soldier = GetGraphics()->CreateFBXMesh("Media/Soldier_animated_jump.fbx", Vector3(20.0f, 0.0f, 0.0f));
		soldier->SetScale(0.1f);
	}

	virtual void RunTest(float dt)
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

	virtual void PostTest()
	{
		GetGraphics()->DeleteMesh(copyrightedTempGuy);
		GetGraphics()->DeleteMesh(soldier);
	}
};