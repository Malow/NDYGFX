#pragma once

#include "Graphics.h"
#include <vector>

class OtherTest
{
private:
	iFBXMesh* zSoldier;
	iFBXMesh* zMaleCharacter;
	iFBXMesh* zBowMesh;
	iMesh* zRotationSample;

public:
	OtherTest() : 
		zRotationSample(0),
		zMaleCharacter(0),
		zBowMesh(0),
		zSoldier(0)
	{

	}

	virtual ~OtherTest() 
	{

	}

	void PreTest();
	void RunTest(float diff);
	void PostTest();
};

void OtherTest::PreTest()
{
	zSoldier = GetGraphics()->CreateFBXMesh("media/Soldier_animated_jump.fbx", Vector3(0.0f, 0.0f, 0.0f));
	zRotationSample = GetGraphics()->CreateMesh("media/models/arrow_v01.obj", Vector3(10.0f, 0.0f, 0.0f));
	zMaleCharacter = GetGraphics()->CreateFBXMesh("media/models/token_anims.fbx", Vector3(0.0f, 0.0f, 0.0f));
	zBowMesh = GetGraphics()->CreateFBXMesh("media/models/bow_anims.fbx", Vector3(0.0f, 0.0f, 0.0f));

	if ( zBowMesh )
	{
		zMaleCharacter->BindMesh("LeftWeapon", zBowMesh);
	}
}

void OtherTest::RunTest(float diff)
{
	if ( GetGraphics()->GetKeyListener()->IsPressed('1') )
	{
		static float shootQueueTimes[] = 
		{
			1.3f,
			5.0f,
			1.3f,
			6.0f,
			0.0f
		};

		if ( zMaleCharacter )
		{
			static const char* shootQueueNames[] = 
			{
				"arch_draw_bow",
				"arch_draw_bow_idle",
				"arch_undraw_bow",
				"arch_equiped_idle"
			};

			zMaleCharacter->SetAnimationQueue(shootQueueNames, shootQueueTimes, 4);
		}

		if ( zBowMesh )
		{
			static const char* shootQueueNames2[] = 
			{
				"arch_draw_bow",
				"arch_draw_bow_idle",
				"arch_undraw_bow",
				"arch_equiped_idle",
			};

			zBowMesh->SetAnimationQueue(shootQueueNames2, shootQueueTimes, 4);
		}
	}

	if ( GetGraphics()->GetKeyListener()->IsPressed('2') )
	{
		zMaleCharacter->RotateAxis(Vector3(1.0f, 1.0f, 0.0f), diff * 0.001f);
		zRotationSample->RotateAxis(Vector3(1.0f, 1.0f, 0.0f), diff * 0.001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('3') )
	{
		zMaleCharacter->RotateAxis(Vector3(1.0f, 1.0f, 0.0f), -diff * 0.001f);
		zRotationSample->RotateAxis(Vector3(1.0f, 1.0f, 0.0f), -diff * 0.001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('4') )
	{
		zMaleCharacter->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), diff * 0.001f);
		zRotationSample->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), diff * 0.001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('5') )
	{
		zMaleCharacter->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), -diff * 0.001f);
		zRotationSample->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), -diff * 0.001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('6') )
	{
		zMaleCharacter->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), diff * 0.001f);
		zRotationSample->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), diff * 0.001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('7') )
	{
		zMaleCharacter->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), -diff * 0.001f);
		zRotationSample->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), -diff * 0.001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('8') )
	{
		zMaleCharacter->Scale(1.0f + diff * 0.0001f);
		zRotationSample->Scale(1.0f + diff * 0.0001f);
		zBowMesh->Scale(1.0f + diff * 0.0001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('9') )
	{
		zMaleCharacter->Scale(1.0f - diff * 0.0001f);
		zRotationSample->Scale(1.0f - diff * 0.0001f);
		zBowMesh->Scale(1.0f - diff * 0.0001f);
	}
	else if ( GetGraphics()->GetKeyListener()->IsPressed('G') )
	{
		zSoldier->HideModel("Plane", !zSoldier->IsModelHidden("Plane"));
	}
	// Warning, can cause crashes
	else if ( GetGraphics()->GetKeyListener()->IsPressed('E') )
	{
		if ( zBowMesh )
		{
			const char* newMesh = 0;
			
			if ( !strcmp(zBowMesh->GetFilePath(), "media/models/bow_anims.fbx") )
			{
				newMesh = "media/models/bow_anims_n45.fbx";
			}
			else if ( !strcmp(zBowMesh->GetFilePath(), "media/models/bow_anims_n45.fbx") )
			{
				newMesh = "media/models/bow_anims_n60.fbx";
			}
			else if ( !strcmp(zBowMesh->GetFilePath(), "media/models/bow_anims_n60.fbx") )
			{
				newMesh = "media/models/bow_anims_p45.fbx";
			}
			else if ( !strcmp(zBowMesh->GetFilePath(), "media/models/bow_anims_p45.fbx") )
			{
				newMesh = "media/models/bow_anims_p60.fbx";
			}
			else if ( !strcmp(zBowMesh->GetFilePath(), "media/models/bow_anims_p60.fbx") )
			{
				newMesh = "media/models/bow_anims.fbx";
			}

			if ( zMaleCharacter ) zMaleCharacter->UnbindMesh(zBowMesh);
			GetGraphics()->DeleteMesh(zBowMesh);
			zBowMesh = 0;

			if ( newMesh )
			{
				zBowMesh = GetGraphics()->CreateFBXMesh(newMesh, Vector3(0.0f, 0.0f, 0.0f));
				if ( zMaleCharacter && newMesh ) zMaleCharacter->BindMesh("LeftWeapon", zBowMesh);
			}
		}

	}
}


void OtherTest::PostTest()
{
	// Delete Rotation Sample
	if ( zRotationSample ) 
	{
		GetGraphics()->DeleteMesh(zRotationSample);
	}

	// Delete Bow
	if ( zBowMesh )
	{
		if ( zMaleCharacter ) zMaleCharacter->UnbindMesh(zBowMesh);
		GetGraphics()->DeleteMesh(zBowMesh);
	}

	// Delete Character
	if ( zMaleCharacter )
	{
		GetGraphics()->DeleteMesh(zMaleCharacter);
	}
}