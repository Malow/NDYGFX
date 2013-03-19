#pragma once

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"

class MaloWTest
{
private:
	iMesh* arrow;
	Vector3 arrowDir;
	iMesh* ball;
	iMesh* secModel;
	iWaterPlane* wp;
	iMesh* deer;
	iMesh* tempguy;
	iMesh* cube;

	iFBXMesh* fbx;

	iMesh* aniTree;
	iMesh* staticTree;
	
	//int nrofdiffs;
	//float totDiff;

public:
	MaloWTest() {};
	virtual ~MaloWTest() {};

	void PreTest();
	void RunTest(float diff);
	void PostTest();
};

void MaloWTest::PreTest()
{
	GetGraphics()->SetEnclosingFogEffect(Vector3(0, 10, 0), 100.0f);
	//nrofdiffs = 0;
	//totDiff = 0;
	wp = GetGraphics()->CreateWaterPlane(Vector3(0, 15, 0), "Media/WaterTexture.png");
	//wp->SetScale(1.0f);
	iWaterPlane* wp2 = GetGraphics()->CreateWaterPlane(Vector3(5, 10, 0), "Media/WaterTexture.png");
	wp2->Rotate(Vector3(0.1f, 0.1f, 0.1f));
	wp2->SetScale(30.0f);

	arrow = GetGraphics()->CreateMesh("Media/Arrow_v01.obj", Vector3(30, 10, 30));					//**ORSAKAR minnesläcka om createmesh använder samma texture OCH skapas FÖRE!
	arrow->Scale(1.0f * 0.05f);
	arrowDir = Vector3(0, 0, -1);

	// test fps
	/*
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, -5));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, 0));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, 5));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 10 + i * 3, 10));
		*/

	ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, -100, 0));
	ball->Scale(0.1f);
	secModel = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(10, 10, 10));
	secModel->Scale(1.0f * 0.05f);

	iMesh* ball2 = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(20, 20, 20));
	ball2->Scale(0.8f);

	//iMesh* temp2 = GetGraphics()->CreateMesh("Media/temp_guy_supertest_anim.fbx", Vector3(-10, 3, 0));
	//temp2->SetScale(0.05f);

	iMesh* tree = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(30, 20, 30));
	tree->Scale(0.05f);

	iMesh* tree2 = GetGraphics()->CreateMesh("Media/TreeTest2.obj", Vector3(32, 20, 30));
	tree2->Scale(0.05f);

	iMesh* fern = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(30, 20, 25));
	fern->Scale(0.05f);

	iMesh* fern2 = GetGraphics()->CreateMesh("Media/Fern_02 - Copy.ani", Vector3(32, 20, 25));
	fern2->Scale(0.05f);

	iMesh* ferntest = GetGraphics()->CreateMesh("Media/fernTest.obj", Vector3(25, 20, 25));
	ferntest->Scale(0.05f);	

	fbx = GetGraphics()->CreateFBXMesh("Media/Soldier_animated_jump.fbx", Vector3(50, 10, 50));
	fbx->Scale(0.1f);

	deer = GetGraphics()->CreateMesh("Media/deer_temp.obj", Vector3(10, 20, 25));
	deer->Scale(0.05f);
	//tempguy = GetGraphics()->CreateMesh("Media/temp_guy.obj", Vector3(15, 20, 25));
	//tempguy->Scale(0.05f);

	staticTree = GetGraphics()->CreateMesh("Media/MapModels/Tree_01_01.obj", Vector3(10, -70, 10));
	staticTree->SetScale(0.05f);
	staticTree = GetGraphics()->CreateMesh("Media/MapModels/Tree_01_01.obj", Vector3(10, 70, 10));
	aniTree = NULL;
	staticTree->SetScale(0.05f);

	aniTree = GetGraphics()->CreateMesh("Media/MapModels/Tree_01.ani", Vector3(10, -70, 10));
	aniTree->SetScale(0.05f);
}

void MaloWTest::RunTest(float diff)
{
	//nrofdiffs++;
	//totDiff += diff;
	/*
	if(diff > 2.0f)
	{
		nrofdiffs = 1;
		totDiff = diff;
	}
	*/
	CollisionData cd;
	//cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
		//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), iAM);
		
	//diff = GetGraphics()->Update();
	//GetGraphics()->GetPhysicsEngine()->GetCollisionRayTerrain(
		//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->GetForward(), iT);
	//diff = GetGraphics()->Update();
	//MaloW::Debug("Normal: " + MaloW::convertNrToString(diff));
	//diff = GetGraphics()->Update();
	//cd = GetGraphics()->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(
		//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), iT, testSize / (vertSize - 1));
	//diff = GetGraphics()->Update();
	//MaloW::Debug("Special: " + MaloW::convertNrToString(diff));
	//MaloW::Debug(" ");
	//diff = 100.0f;

	

	//secModel->MoveBy(Vector3(0.001f, 0.001f, 0.001f) * diff);

	/*
	// Simulate distance checking
	for(int i = 0; i < 10000; i++)
	{
		Vector3 to = GetGraphics()->GetCamera()->GetPosition() - wp->GetPosition();
		if(to.GetLength() < 10.0f)
			float asd = to.GetLength();
	}

	// 1/5'th of the models hits.
	for(int i = 0; i < 2000; i++)*/
	//secModel->SetPosition(Vector3(10, 20, 10));
	//GetGraphics()->GetCamera()->SetPosition(secModel->GetPosition() + Vector3(5, 0, 1));
	//GetGraphics()->GetCamera()->LookAt(secModel->GetPosition() + Vector3(0, 0, 1));
	//Vector3 asd = GetGraphics()->GetCamera()->Get3DPickingRay();

	
	//cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(GetGraphics()->GetCamera()->GetPosition(), 
	//	GetGraphics()->GetCamera()->GetForward(), tempguy);
	
	//if(!cd.collision)
		//cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(GetGraphics()->GetCamera()->GetPosition(), 
		//GetGraphics()->GetCamera()->Get3DPickingRay(), deer);
	if(GetGraphics()->GetKeyListener()->IsPressed('1'))
		fbx->RotateAxis(Vector3(0, 1, 0), diff * 0.01f);
	
	fbx->SetAnimation((unsigned int)0);
	cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), fbx);
	/*
	cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(GetGraphics()->GetCamera()->GetPosition(), 
		GetGraphics()->GetCamera()->Get3DPickingRay(), cube);
		*/

	if(cd.BoundingSphereCollision)
	{
		ball->SetScale(0.2f);
	}
	else
	{
		ball->SetScale(0.1f);
	}

	if(cd.collision)
	{
		ball->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
	}






	static bool fesd = true;
	static int qual = 0;
	if(GetGraphics()->GetKeyListener()->IsPressed('H'))
	{
		if(fesd)
		{
			if(qual % 2 == 0)
			{
				//GetGraphics()->GetEngineParameters().Maximized = true;
				//GetGraphics()->ResizeGraphicsEngine(1280, 1024);
				wp->SetVertexPosition(Vector3(1, 0, -0.1), 0);
				wp->SetVertexPosition(Vector3(0.8f, 0.2f, 0.8f), 1);
				wp->SetVertexPosition(Vector3(-1, -0.2f, -0.2f), 2);
				wp->SetVertexPosition(Vector3(-0.8f, 0, 1), 3);

				
				aniTree = GetGraphics()->CreateMesh("Media/MapModels/Tree_01.ani", Vector3(10, 70, 10));
				aniTree->SetScale(0.05f);
				GetGraphics()->DeleteMesh(staticTree);
			}
			else
			{
				//GetGraphics()->GetEngineParameters().Maximized = false;
				//GetGraphics()->ResizeGraphicsEngine(1264, 947);
				wp->SetVertexPosition(Vector3(0.5f, 0, -0.5f), 0);
				wp->SetVertexPosition(Vector3(0.5f, 0, 0.5f), 1);
				wp->SetVertexPosition(Vector3(-0.5f, 0, -0.5f), 2);
				wp->SetVertexPosition(Vector3(-0.5f, 0, 0.5f), 3);

				
				staticTree = GetGraphics()->CreateMesh("Media/MapModels/Tree_01_01.obj", Vector3(10, 70, 10));
				staticTree->SetScale(0.05f);
				GetGraphics()->DeleteMesh(aniTree);
			}
			//GetGraphics()->ReloadShaders(21);
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


	if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
		secModel->MoveBy(Vector3(1, 0, 0) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
		secModel->MoveBy(Vector3(-1, 0, 0) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
		secModel->MoveBy(Vector3(0, 0, 1) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
		secModel->MoveBy(Vector3(0, 0, -1) * diff * 0.1f);



	if(GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
		secModel->MoveBy(Vector3(0, 1, 0) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
		secModel->MoveBy(Vector3(0, -1, 0) * diff * 0.1f);


	if(GetGraphics()->GetKeyListener()->IsClicked(1))
	{
		arrow->SetPosition(GetGraphics()->GetCamera()->GetPosition());

		Vector3 vec(0, 0, -1);

		Vector3 camDir = GetGraphics()->GetCamera()->GetForward();
		arrowDir = camDir;

		arrow->ResetRotation();
		Vector3 around = vec.GetCrossProduct(camDir);
		float angle = acos(vec.GetDotProduct(camDir) / (vec.GetLength() * camDir.GetLength()));
		arrow->RotateAxis(around, angle);
	}
	arrow->MoveBy(arrowDir * diff * 0.01f);



	if(GetGraphics()->GetKeyListener()->IsPressed('9'))
	{
		GetGraphics()->GetEngineParameters().FarClip *= (1.0f + diff * 0.002f);
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('8'))
	{
		GetGraphics()->GetEngineParameters().FarClip *= (1.0f - diff * 0.002f);
	}

	if(GetGraphics()->GetKeyListener()->IsPressed('O'))
	{
		GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), GetGraphics()->GetSunLightColor() * (1.0f + diff * 0.002f));
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('P'))
	{
		GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), GetGraphics()->GetSunLightColor() * (1.0f - diff * 0.002f));
	}
}

void MaloWTest::PostTest()
{

}