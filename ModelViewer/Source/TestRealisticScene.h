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
	LoadEntList("Entities.txt");
	world = new World(this, "Media/Maps/Map_01_v63.map", true);
	GetGraphics()->GetCamera()->SetPosition(Vector3(world->GetWorldCenter().x, 20.0f, world->GetWorldCenter().y));
	path = 0;

	
	Vector3 camPos = GetGraphics()->GetCamera()->GetPosition();
	
	for(int i = 0; i < 50; i++)
	{
		iWaterPlane* wp = GetGraphics()->CreateWaterPlane(camPos + Vector3(i * 5, 0, 0), "Media/WaterTexture.png");
		wp->SetScale(10.0f);
	}

	GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(50, -10, 50));
	GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(-50, -10, 50));
	GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(-50, -10, -50));
	GetGraphics()->CreateFBXMesh("Media/temp_guy_few_anims.fbx", camPos + Vector3(50, -10, -50));

	for(int i = 0; i < 50; i++)
	{
		iDecal* wp = GetGraphics()->CreateDecal(camPos + Vector3(i * 5, -10, 0), "Media/BloodTexture.png", Vector3(0,-1,0), Vector3(1, 0, 0));
	}
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
		if(GetGraphics()->GetCamera()->GetPosition().x > 2000)
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
	
	return false;
}

void TestRealisticScene::PostTest()
{
	delete world;
	delete wr;
}