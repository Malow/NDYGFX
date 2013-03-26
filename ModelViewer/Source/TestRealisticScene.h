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
	float enclosingfog;
	Vector3 fogCenter;
	iLight* l1;

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

	unsigned int nrOfResources = 6;
	const char* resourceFileNames[6];
	resourceFileNames[0] = "Media/MapModels/Tree_02.ani";
	resourceFileNames[1] = "Media/MapModels/Tree_02_01.obj";			
	resourceFileNames[2] = "Media/MapModels/Bush_01.ani";
	resourceFileNames[3] = "Media/MapModels/Bush_01_01.obj";		
	resourceFileNames[4] = "Media/MapModels/Tree_01.ani";	
	resourceFileNames[5] = "Media/MapModels/Tree_01_01.obj";	
	GetGraphics()->PreLoadResources(nrOfResources, resourceFileNames);
	
	LoadEntList("Entities.txt");
	world = new World(this, "Media/Maps/Map_01_v87.map", true);
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
	this->enclosingfog = 300.0f;
	this->fogCenter = camPos;
	GetGraphics()->SetEnclosingFogEffect(this->fogCenter, this->enclosingfog);
	
	//GetGraphics()->SetSunLightProperties(Vector3(0.801525652, -0.401229233, -0.443363965));
	
	for(int i = 0; i < 50; i++)
	{
		iWaterPlane* wp = GetGraphics()->CreateWaterPlane(camPos + Vector3(i * 5, 0, 0), "Media/WaterTexture.png");
		wp->SetScale(10.0f);
	}
	
	//iWaterPlane* wp = GetGraphics()->CreateWaterPlane(Vector3(0, 0, 0), "Media/WaterTexture.png");
	//wp->SetVertexPosition(Vector3(300, -35, 100) + camPos, 0);
	//wp->SetVertexPosition(Vector3(300, -35, 300) + camPos, 1);
	//wp->SetVertexPosition(Vector3(100, -15, 100) + camPos, 2);
	//wp->SetVertexPosition(Vector3(100, -15, 300) + camPos, 3);
	
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

	iFBXMesh* me = GetGraphics()->CreateFBXMesh("Media/Models/token_anims.fbx", camPos + Vector3(50, -10, 50));
	me->SetPosition(groundPos + Vector3(3, 0, 0));
	me->SetScale(0.05f);
	
	for(int i = 0; i < 50; i++)
	{
		Vector3 decPos = camPos + Vector3(i * 2, -10, 0);
		decPos.y = world->GetHeightAt(Vector2(decPos.x, decPos.z));
		iDecal* wp = GetGraphics()->CreateDecal(decPos + Vector3(0.5, 0, 0.5), "Media/token_print_l.png", Vector3(0,-1,0), Vector3(1, 0, 0));
		wp->SetOpacity(0.3f);
		iDecal* wp2 = GetGraphics()->CreateDecal(decPos - Vector3(0.5, 0, 0.5), "Media/token_print_r.png", Vector3(0,-1,0), Vector3(1, 0, 0));
		wp2->SetOpacity(0.3f);
		if(i == 0)
		{
			iDecal* wp3 = GetGraphics()->CreateDecal(decPos - Vector3(2, 0, 1), "Media/BloodTexture.png", Vector3(0,-1,0), Vector3(1, 0, 0));
			wp3->SetOpacity(0.6f);
			wp3->SetSize(2.0f);
		}
	}
	
	GetGraphics()->SetGrassFilePath("Media/Grass.png");

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);
	

	me->SetAnimation("idle_04_feed");
	me->SetPosition(me->GetPosition() + Vector3(30, 0, 10));
	//GetGraphics()->GetCamera()->SetMesh(me, "Neck", Vector3(0, 0, 1));



	/*
	l1 = GetGraphics()->CreateLight(Vector3(45, 5, 45));
	l1->SetLookAt(Vector3(45, 0, 45));
	l1->SetUp(Vector3(1, 0, 0));
	l1->SetIntensity(10.0f);
	l1->SetColor(Vector3(1.0f, 0.9f, 0.0f));*/
}

bool TestRealisticScene::RunTest(float diff)
{
	/*
	static Vector3 sunDirTest = Vector3(1.0f, -2.0f, 1.0f);
	static float radius = 0.0f;
	radius += diff * 0.00000001f;
	sunDirTest.RotateY(radius);
	GetGraphics()->SetSunLightProperties(sunDirTest);
	*/
	//l1->SetPosition(GetGraphics()->GetCamera()->GetPosition());

	if(GetGraphics()->GetKeyListener()->IsPressed('R'))
	{
		GetGraphics()->ReloadShaders(11);
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('4'))
	{
		GetGraphics()->GetEngineParameters().FOV += diff * 0.01f;
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('5'))
	{
		GetGraphics()->GetEngineParameters().FOV -= diff * 0.01f;
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('6'))
	{
		GetGraphics()->GetEngineParameters().FarClip += diff * 0.01f;
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('7'))
	{
		GetGraphics()->GetEngineParameters().FarClip -= diff * 0.01f;
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('8'))
	{
		this->enclosingfog += diff * 0.1f;
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('9'))
	{
		this->enclosingfog -= diff * 0.1f;
	}
	GetGraphics()->SetEnclosingFogEffect(this->fogCenter, this->enclosingfog);

	 // Print Screen comparison
	if(GetGraphics()->GetKeyListener()->IsPressed('0'))
	{
		GetGraphics()->GetCamera()->SetPosition(Vector3(1986, 2, 1974));
		GetGraphics()->GetCamera()->LookAt(Vector3(2000, 2, 1980));
	}


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
			Vector3 asd = GetGraphics()->GetCamera()->GetForward();
			int qweqwe = 0;


			if(qual % 2 == 0)
			{
				GetGraphics()->ChangeSkyBox("Media/skymap.dds"); 
				GetGraphics()->SetFogColor(Vector3(0.45f, 0.45f, 0.45f));
			}
			else
			{
				GetGraphics()->ChangeSkyBox("Media/StarMap.dds"); 
				GetGraphics()->SetFogColor(Vector3(0.1f, 0.1f, 0.1f));
			}
			
			//GetGraphics()->ReloadShaders(11);
			//GetGraphics()->ReloadShaders(27);
			//GetGraphics()->ReloadShaders(16);
			//GetGraphics()->ReloadShaders(19);
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