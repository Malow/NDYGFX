#pragma once

#define AUTO_PATH  // Comment out for manual movement.

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"
#include "World\EntityList.h"
#include "World\Observer.h"
#include "World\World.h"
#include "World\WorldRenderer.h"

class TestRealisticScene : public Observer
{
private:
	World* world;
	WorldAnchor* wa;
	WorldRenderer* wr;
	int path;

public:
	TestRealisticScene() {};
	virtual ~TestRealisticScene() {};

	void PreTest();
	bool RunTest(float diff);
	void PostTest();

protected:
	virtual void OnEvent(Event* e) 
	{
		if ( WorldDeletedEvent* wde = dynamic_cast<WorldDeletedEvent*>(e))
		{
			delete this->wr;
			this->wr = NULL;
		}
		if ( WorldLoadedEvent* WLE = dynamic_cast<WorldLoadedEvent*>(e) )
		{
			// Create Anchor
			wa = WLE->world->CreateAnchor();
			this->wr = new WorldRenderer(WLE->world, GetGraphics());
		}
	}
};

void TestRealisticScene::PreTest()
{
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);

	LoadEntList("Entities.txt");
	world = new World(this, "Media/Maps/Map_01_v85.map", true);
	GetGraphics()->GetCamera()->SetPosition(Vector3(world->GetWorldCenter().x, 20.0f, world->GetWorldCenter().y));
	path = 0;

	wa->position = GetGraphics()->GetCamera()->GetPosition().GetXZ();
	wa->radius = GetGraphics()->GetEngineParameters().FarClip;
	world->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();
	wr->Update();

	
	Vector3 camPos = GetGraphics()->GetCamera()->GetPosition();
	
	for(int i = 0; i < 50; i++)
	{
		iWaterPlane* wp = GetGraphics()->CreateWaterPlane(camPos + Vector3(i * 5, 0, 0), "Media/WaterTexture.png");
		wp->SetScale(10.0f);
	}
	/*
	iWaterPlane* wp = GetGraphics()->CreateWaterPlane(Vector3(0, 0, 0), "Media/WaterTexture.png");
	wp->SetVertexPosition(Vector3(300, -35, 100) + camPos, 0);
	wp->SetVertexPosition(Vector3(300, -35, 300) + camPos, 1);
	wp->SetVertexPosition(Vector3(100, -15, 100) + camPos, 2);
	wp->SetVertexPosition(Vector3(100, -15, 300) + camPos, 3);
	*/
	iMesh* meee = GetGraphics()->CreateFBXMesh("Media/Models/token_anims.fbx", camPos + Vector3(50, -10, 50));
	meee->SetScale(0.5f);
	meee = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(-50, -10, 50));
	meee->SetScale(0.5f);
	meee = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(-50, -10, -50));
	meee->SetScale(0.5f);
	meee = GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(50, -10, -50));
	meee->SetScale(0.5f);

	meee = GetGraphics()->CreateMesh("Media/Models/NMTest.obj", camPos + Vector3(-20, -10, 0));
	meee->SetScale(1.0f);
	
	meee = GetGraphics()->CreateMesh("Media/MaloWGravestone.obj", camPos);
	Vector3 groundPos = camPos;
	groundPos.x += 70;
	groundPos.y = world->GetHeightAt(Vector2(groundPos.x, groundPos.z));
	meee->SetPosition(groundPos);
	meee->SetScale(0.05f);

	meee = GetGraphics()->CreateFBXMesh("Media/Models/token_anims.fbx", camPos + Vector3(50, -10, 50));
	meee->SetPosition(groundPos + Vector3(3, 0, 0));
	meee->SetScale(0.05f);
	
	for(int i = 0; i < 50; i++)
	{
		iDecal* wp = GetGraphics()->CreateDecal(camPos + Vector3(i * 5, -10, 0), "Media/BloodTexture.png", Vector3(0,-1,0), Vector3(1, 0, 0));
	}

	GetGraphics()->SetGrassFilePath("Media/Grass.png");

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);
	//GetGraphics()->GetCamera()->SetMesh(meee, "Neck", Vector3(0, 0, 1));
}

bool TestRealisticScene::RunTest(float diff)
{
	wa->position = GetGraphics()->GetCamera()->GetPosition().GetXZ();
	wa->radius = GetGraphics()->GetEngineParameters().FarClip;
	world->Update();
	wr->Update();
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_BACK))
	{
		GetGraphics()->ResetPerfLogging();
	}
#ifdef AUTO_PATH
	Vector3 pos;
	if(path == 0)
	{
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RETURN))
		{
			GetGraphics()->GetCamera()->SetPosition(Vector3(world->GetWorldCenter().x, 20.0f, world->GetWorldCenter().y));
			path++;
		}
	}
	else if(path == 1)
	{
		float diffylol = 0.0f;
		while(diffylol < 3000.0f)
		{
			diffylol += GetGraphics()->Update();
			wa->position = GetGraphics()->GetCamera()->GetPosition().GetXZ();
			wa->radius = GetGraphics()->GetEngineParameters().FarClip;
			world->Update();
			wr->Update();
		}
		GetGraphics()->ResetPerfLogging();
		path++;
	}
	else if(path == 2)
	{
		GetGraphics()->GetCamera()->SetForward(Vector3(1, 0, 1));
		GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		pos = GetGraphics()->GetCamera()->GetPosition();
		pos.y = world->GetHeightAt(Vector2(pos.x, pos.z)) + 5.0f;
		GetGraphics()->GetCamera()->SetPosition(pos);
		if(GetGraphics()->GetCamera()->GetPosition().x > 2100)
			path++;
		
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RETURN))
		{
			return true;
		}
	}
	else if(path == 3)
	{
		GetGraphics()->GetCamera()->SetForward(Vector3(-1, 0, -1));
		GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		pos = GetGraphics()->GetCamera()->GetPosition();
		pos.y = world->GetHeightAt(Vector2(pos.x, pos.z)) + 5.0f;
		GetGraphics()->GetCamera()->SetPosition(pos);
		if(GetGraphics()->GetCamera()->GetPosition().x < 1700)
			path++;

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RETURN))
		{
			return true;
		}
	}
	else
		return true;
#endif
	










	static bool fesd = true;
	static int qual = 0;
	if(GetGraphics()->GetKeyListener()->IsPressed('H'))
	{
		if(fesd)
		{

			if(qual % 2 == 0)
			{
				
			}
			else
			{
				
			}
			GetGraphics()->ReloadShaders(22);
			GetGraphics()->ReloadShaders(23);
			//GetGraphics()->ReloadShaders(1);
			//MaloW::Debug("Diff: " + MaloW::convertNrToString(totDiff / nrofdiffs));
			//GetGraphics()->ChangeShadowQuality(qual);
			//secModel->SetPosition(Vector3(10, 10, 10));
			qual++;
			fesd = false;
		}			
	}
	else
		fesd = true;





	return false;
}

void TestRealisticScene::PostTest()
{
	GetGraphics()->PrintPerfLogging();
	delete world;
}