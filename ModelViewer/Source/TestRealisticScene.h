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

public:
	TestRealisticScene() {};
	virtual ~TestRealisticScene() {};

	void PreTest();
	void RunTest(float diff);
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
}

void TestRealisticScene::RunTest(float diff)
{
	wa->position = GetGraphics()->GetCamera()->GetPosition().GetXZ();
	wa->radius = GetGraphics()->GetEngineParameters().FarClip;
	world->Update();
	wr->Update();
}

void TestRealisticScene::PostTest()
{
	delete world;
	delete wr;
}