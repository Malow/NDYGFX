#pragma once

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
	//wa = world->CreateAnchor();
	GetGraphics()->GetCamera()->SetPosition(Vector3(world->GetWorldCenter().x, 20.0f, world->GetWorldCenter().y));
	path = 0;
}

bool TestRealisticScene::RunTest(float diff)
{
	wa->position = GetGraphics()->GetCamera()->GetPosition().GetXZ();
	wa->radius = GetGraphics()->GetEngineParameters().FarClip;
	world->Update();
	wr->Update();

	Vector3 pos;
	if(path == 0)
	{
		GetGraphics()->GetCamera()->SetForward(Vector3(1, 0, 1));
		GetGraphics()->GetCamera()->MoveForward(diff * 5.0f);
		pos = GetGraphics()->GetCamera()->GetPosition();
		pos.y = world->GetHeightAt(Vector2(pos.x, pos.z)) + 5.0f;
		GetGraphics()->GetCamera()->SetPosition(pos);
		if(GetGraphics()->GetCamera()->GetPosition().x > 2000)
			path++;
	}
	else if(path == 1)
	{
		GetGraphics()->GetCamera()->SetForward(Vector3(-1, 0, -1));
		GetGraphics()->GetCamera()->MoveForward(diff * 5.0f);
		pos = GetGraphics()->GetCamera()->GetPosition();
		pos.y = world->GetHeightAt(Vector2(pos.x, pos.z)) + 5.0f;
		GetGraphics()->GetCamera()->SetPosition(pos);
		if(GetGraphics()->GetCamera()->GetPosition().x < 1700)
			path++;
	}
	else
		return true;
	
	return false;
}

void TestRealisticScene::PostTest()
{
	delete world;
	delete wr;
}