#pragma once

#include "Graphics.h"


class TestAnimationQueue
{
	iFBXMesh* zMaleCharacter;
	iMesh* zBowMesh;

public:
	TestAnimationQueue()
	{

	}

	~TestAnimationQueue()
	{

	}

	void PreTest()
	{
		zMaleCharacter = GetGraphics()->CreateFBXMesh("media/models/token_anims.fbx", Vector3(0.0f, 0.0f, 0.0f));
		zBowMesh = GetGraphics()->CreateMesh("media/models/bow_v01.obj", Vector3(0.0f, 0.0f, 0.0f));
		if ( zBowMesh ) zMaleCharacter->BindMesh("LeftWeapon", zBowMesh);
	}

	void RunTest(float diff)
	{
		if ( GetGraphics()->GetKeyListener()->IsPressed('1') )
		{
			static const char* shootQueueNames[] = 
			{
				"arch_equip",
				"arch_equiped_idle",
				"arch_unequip",
				"idle_03_zombie"
			};

			static float shootQueueTimes[] = 
			{ 
				1.6,
				4.8f,
				1.6f,
				0.0f
			};

			zMaleCharacter->SetAnimationQueue(shootQueueNames, shootQueueTimes, 4);
		}
	}

	void PostTest()
	{
		// Delete Bow
		if ( zBowMesh ) 
		{
			zMaleCharacter->UnbindMesh(zBowMesh);
			GetGraphics()->DeleteMesh(zBowMesh);
		}

		// Delete Character
		GetGraphics()->DeleteFBXMesh(zMaleCharacter);
	}
};