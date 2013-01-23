#define TEST //<----------------------- kommentera ut vid behov **********************


#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
	#define INCLUDE_MODEL_VIEWER
#endif

#include "Graphics.h"
#include "..\..\NDYGFX\Source\MaloWFileDebug.h"
//#include "MaloWFileDebug.h"

void ReplaceSlashes(string& str, char replace, char with)
{
	for(unsigned int i = 0; i < str.size(); i++)
	{
		if(str.at(i) == replace)
			str.at(i) = with;
	}
}

// Does work, not when running through visual studio tho since it fucks up the path that GetModuleFileName gives.
void deleteCache()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	string path = string(szPath);
	path = path.substr(0, path.size() - string("ModelViewer.exe").size());
	path += "\\Media\\Cache\\*";
	MaloW::Debug(path);

	int len = strlen(path.c_str()) + 2; // required to set 2 nulls at end of argument to SHFileOperation.
	char* tempdir = (char*) malloc(len);
	memset(tempdir,0,len);
	strcpy(tempdir,path.c_str()); //**tillman**

	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		tempdir,
		"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		"" };

	if ( int ret = SHFileOperation(&file_op) )
	{
		throw("Error!");
	}

	free(tempdir);
}

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, int )
{
	MaloW::ClearDebug();
	if ( !GraphicsInit(hInstance) )
		MaloW::Debug("Failed Creating Graphics Engine!");
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MaloW::Debug("(DEBUG): ModelViewer: Debug flag set to: _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF). ");
	#ifdef INCLUDE_MODEL_VIEWER
		MaloW::Debug("(DEBUG): ModelViewer: vld.h included.");
	#endif
#endif

	GetGraphics()->CreateSkyBox("Media/skymap.dds");	// Reduces FPS from 130 to 40 //** TILLMAN
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	iLight* li = GetGraphics()->CreateLight(GetGraphics()->GetCamera()->GetPosition());
	li->SetIntensity(0.001f);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	

//*************************************	     PRE TEST       **********************
#ifdef TEST

	int vertSize = 16;
	float testSize = 50.0f;
	
	iTerrain* iT = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(testSize, 0.0f, testSize), vertSize);
	iTerrain* iT2 = GetGraphics()->CreateTerrain(Vector3(testSize, 0, 0), Vector3(testSize, 0.0f, testSize), vertSize);
	
	iAnimatedMesh* iAM = GetGraphics()->CreateAnimatedMesh("Media/FlagBlue.ani", Vector3(30, 30, 30));
	//iImage* iM = GetGraphics()->CreateImage(Vector2(100, 100), Vector2(100, 100), "Media/BallTexture.png");
	//iText* iTe = GetGraphics()->CreateText("durp", Vector2(300, 100), 1.0f, "Media/Fonts/1");
	
	float test = 0.0f;
	Vector2 dd = Vector2(0.1f, 0.2f);
	test = dd[0];
	Vector3 durp = Vector3(1.0f, 2.0f, 3.0f);
	test = durp[2];
	Vector4 hurp = Vector4(10.0f, 20.0f, 30.0f, 40.0f);
	test = hurp[3];

	float inner = 1.0f;
	float outer = inner * 2;
	GetGraphics()->SetSpecialCircle(inner, outer, Vector2(0, 0));
	float hmData[33 * 33];
	for(int i = 0; i < vertSize * vertSize; i++)
	{
		/*hmData[0] = 0.0f;
		hmData[1] = 0.0f;
		hmData[2] = 2.0f;
		hmData[3] = 0.0f;*/
		hmData[i] = 0.0f;
	}
	iT->SetHeightMap(hmData);
	const char* fileNames[4];
	fileNames[0] = "Media/BallTexture.png";
	fileNames[1] = "Media/TerrainTexture.png";
	fileNames[2] = "Media/TerrainTexture.png";
	fileNames[3] = "Media/TerrainTexture.png";
	iT->SetTextures(fileNames);
	iT->SetHeightMap(hmData);
	iT->SetTextureScale(-4);

	float testF = 0.0f;
	try
	{
		testF = iT->GetYPositionAt(0.5f, 0.5f);
	}
	catch(...)
	{
	}
	
	
	int size = 2048;
	float* testData = new float[size*size*4]; 
	for(int i = 0; i < size*size; i++)
	{
		testData[ i * 4 + 0 ] = 1.0f;
		testData[ i * 4 + 1 ] = 1.0f;
		testData[ i * 4 + 2 ] = 1.0f;
		testData[ i * 4 + 3 ] = 1.0f;
	}

	iT->SetBlendMap(size, testData);


	fileNames[0] = "Media/BallTexture.png";
	fileNames[1] = "Media/TerrainTexture.png";
	fileNames[2] = "Media/TerrainTexture.png";
	fileNames[3] = "Media/TerrainTexture.png";
	iT2->SetTextures(fileNames);
	iT2->SetBlendMap(size, testData);

	int klerp = 10;
	iTerrain** iTs = new iTerrain*[klerp];
	for(int i = 0; i < klerp; i++)
	{
		iTs[i] = GetGraphics()->CreateTerrain(Vector3((testSize * 2) + (i * testSize), 0, 0), Vector3(testSize, 0.0f, testSize), vertSize);
		iTs[i]->SetTextures(fileNames);
		iTs[i]->SetBlendMap(size, testData);
		iTs[i]->SetTextureScale(10);
	}
	
	// test fps
//for(int i = 0; i < 50; i++)
	//	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, 10 + i * 3, 0));
	//for(int i = 0; i < 50; i++)
	//	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(10, 10 + i * 3, 0));
	//for(int i = 0; i < 50; i++)
	//	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(10, 10 + i * 3, 10));
	//for(int i = 0; i < 50; i++)
	//	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, 10 + i * 3, 10));
	
	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, -100, 0));
	ball->Scale(0.1f);
	iMesh* secModel = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(10, 2, 10));
	secModel->Scale(1.0f * 0.05f);

	iMesh* bush = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(30, 10, 30));
	bush->Scale(1.0f * 0.05f);

	iMesh* arrow = GetGraphics()->CreateMesh("Media/Arrow_v01.obj", Vector3(30, 10, 30));
	arrow->Scale(1.0f * 0.05f);
	Vector3 arrowDir = Vector3(0, 0, -1);



	//CASCADED SHADOWMAP:
	iMesh* bushCSMTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(10, 2, 0));
	bushCSMTest->Scale(1.0f * 0.05f);
	iMesh* bushScaleBugTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(0, 0, 0));
	iMesh* scaleBugTest = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(5, 0, 0));
	iMesh* bushScaleBugTest2 = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(2, 0, 0));
	iMesh* scaleBugTest2 = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(4, 0, 0));

	bushScaleBugTest->Scale(1.0f * 0.05f);
	scaleBugTest->Scale(1.0f * 0.05f);
	bushScaleBugTest2->Scale(1.0f * 0.05f);
	scaleBugTest2->Scale(1.0f * 0.25f);
	/*
	Vector3 lookAt = Vector3(0, 0, 0);
	Vector3 pos = Vector3(-50, 50, -50);
	Vector3 posToLookAt = lookAt - pos; //Pos --> lookAt
	Vector3 dir;
	Vector3 up = Vector3(0, 1, 0);
	//D3DXVec3Normalize(&dir, &posToLookAt); 
	posToLookAt.Normalize();
	dir = posToLookAt;
	GetGraphics()->SetSunLightProperties(dir);
	//Set camera to look along the x-axis
	GetGraphics()->GetCamera()->SetPosition(Vector3(0, 1, 0));
	GetGraphics()->GetCamera()->LookAt(Vector3(1, 1, 0));

	//calculate points (in world space) for the frustum slice
	Vector3 camPos = GetGraphics()->GetCamera()->GetPosition();
	Vector3 camForward = GetGraphics()->GetCamera()->GetForward();
	Vector3 camRight = GetGraphics()->GetCamera()->GetRightVector();
	Vector3 camUp = GetGraphics()->GetCamera()->GetUpVector();
	float ww = (float)GetGraphics()->GetEngineParameters()->windowWidth;
	float hh = (float)GetGraphics()->GetEngineParameters()->windowHeight;
	float aspectRatio = ww / hh;
	float tmp = tan(GetGraphics()->GetEngineParameters()->FOV) * 2;

	float planes[4];
	planes[0] = GetGraphics()->GetEngineParameters()->NearClip;
	planes[1] = GetGraphics()->GetEngineParameters()->FarClip * 0.1f;
	planes[2] = GetGraphics()->GetEngineParameters()->FarClip * 0.4f;
	planes[3] = GetGraphics()->GetEngineParameters()->FarClip;

	//**vertices to render as debug info**
	int nrOfFrustumSlices = 3;
	int ttte = 8;
	Vector3* vertices = new Vector3[ttte * nrOfFrustumSlices];

	for(int i = 0; i < nrOfFrustumSlices; i++)
	{
		//Near plane	
		float halfNearHeight = tmp * planes[i];
		float halfNearWidth = halfNearHeight * aspectRatio;

		//Near points
		Vector3	nearCenter = camPos + camForward * planes[i]; //near plane of slice
		Vector3	nearTopLeft = nearCenter + (camUp * halfNearHeight) - (camRight * halfNearWidth);
		Vector3	nearTopRight = nearCenter + (camUp * halfNearHeight) + (camRight * halfNearWidth);
		Vector3	nearBottomLeft = nearCenter - (camUp * halfNearHeight) - (camRight * halfNearWidth);
		Vector3	nearBottomRight = nearCenter - (camUp * halfNearHeight) + (camRight * halfNearWidth);

		/*vertices[i * ttte] = nearCenter;
		vertices[i * ttte + 1] = nearTopLeft;
		vertices[i * ttte + 2] = nearTopRight;
		vertices[i * ttte + 3] = nearBottomLeft;
		vertices[i * ttte + 4] = nearBottomRight;*/
		/*vertices[i * ttte] = nearTopLeft;
		vertices[i * ttte + 1] = nearTopRight;
		vertices[i * ttte + 2] = nearBottomLeft;
		vertices[i * ttte + 3] = nearBottomRight;
		
		//Far plane
		float halfFarHeight = tmp * planes[i + 1];
		float halfFarWidth = halfFarHeight * aspectRatio;
		//Far points
		Vector3 farCenter = camPos + camForward * planes[i + 1]; //far plane of slice
		Vector3 farTopLeft = farCenter + (camUp * halfFarHeight) - (camRight * halfFarWidth);
		Vector3	farTopRight = farCenter + (camUp * halfFarHeight) + (camRight * halfFarWidth);
		Vector3	farBottomLeft = farCenter - (camUp * halfFarHeight) - (camRight * halfFarWidth);
		Vector3	farBottomRight = farCenter - (camUp * halfFarHeight) + (camRight * halfFarWidth);

		/*vertices[i * ttte + 5] = farCenter;
		vertices[i * ttte + 6] = farTopLeft;
		vertices[i * ttte + 7] = farTopRight;
		vertices[i * ttte + 8] = farBottomLeft;
		vertices[i * ttte + 9] = farBottomRight;*/
		/*vertices[i * ttte + 4] = farTopLeft;
		vertices[i * ttte + 5] = farTopRight;
		vertices[i * ttte + 6] = farBottomLeft;
		vertices[i * ttte + 7] = farBottomRight;
	}
	*/

	//Transform points into light’s homogeneous space.
	//GetGraphics()->DebugDummyFunction(vertices);

	//render
	/*iMesh** debugCSMPoints = new iMesh*[ttte * nrOfFrustumSlices];
	for(int i = 0; i < ttte * nrOfFrustumSlices; i++)
	{
		//debugCSMPoints[i] = GetGraphics()->CreateStaticMesh("Media/ball.obj", vertices[i]); //Camera frustum corners
	}
	//debugCSMPoints[0]->SetSpecialColor(GREEN_COLOR); //min
	//debugCSMPoints[1]->SetSpecialColor(BLUE_COLOR); //max

	//GetGraphics()->CreateStaticMesh("Media/CSMDebug.obj", Vector3(0, 0, 0)); //Camera frustum slices
	
	//restore camera settings
	/*GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	*/
	//for(int i = 0; i < 10; i++)
		//for(int u = 0; u < 10; u++)
			//GetGraphics()->CreateMesh("Media/Asdbfd.obj", Vector3(0 + i * 5, 5, 0 + u * 5))->SetScale(1.0f * 0.05f);
	
	//scale texture debugging(fixed)
	/*
	iMesh* fernDbg1 = GetGraphics()->CreateMesh("Media/Fern_02_v01.obj", Vector3(2, 0, 2));
	iMesh* scaleDbg = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(0, 0, 0));
	fernDbg1->Scale(1.0f / 20.0f);
	scaleDbg->Scale(1.0f * 0.05f);
	*/


#endif
//*************************************	    END OF PRE TEST       **********************

	

	iMesh* scaleHuman = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	scaleHuman->Scale(1.0f / 20.0f);
	model->Scale(1.0f * 0.05f);
#ifdef TEST
	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png"
		, 1.0f, 1.0f, 1.0f, 1.0f);
#endif

	GetGraphics()->StartRendering();	// To force the engine to render a black image before it has loaded stuff to not get a clear-color rendered before skybox is in etc.

	string lastSpecString = "";
	bool showscale = false;
	bool toggleScale = true;
	bool toggleDelCache = true;
	bool toggleLight = true;
	bool go = true;
	float tempInt = 10.0f;
	while(GetGraphics()->IsRunning() && go)
	{
		Sleep(10);
		string specString = GetGraphics()->GetSpecialString();
		ReplaceSlashes(specString, '\\', '/');
		if(specString != lastSpecString)
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			string path = string(szPath);
			path = path.substr(0, path.size() - string("ModelViewer.exe").size());

			string loadModel = specString.substr(path.size() , specString.size());
			GetGraphics()->DeleteMesh(model);
			if(loadModel.substr((loadModel.size() - 4), loadModel.size()) == ".obj")
				model = GetGraphics()->CreateMesh(loadModel.c_str(), Vector3(15, 20, 20));
			else if(loadModel.substr((loadModel.size() - 4), loadModel.size()) == ".ani")
				model = GetGraphics()->CreateAnimatedMesh(loadModel.c_str(), Vector3(15, 20, 20));
			model->Scale(1.0f / 20.0f);
			lastSpecString = specString;
		}

		// Updates camera etc, does NOT render the frame, another process is doing that, so diff should be very low.
		float diff = GetGraphics()->Update();	
		

//*************************************	     RUN TESTS       **********************
#ifdef TEST
		CollisionData cd;
		cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
			GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), iAM);
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
		else
		{
			//ball->SetPosition(Vector3(0, -100, 0));
		}
		
		static bool fesd = true;
		if(GetGraphics()->GetKeyListener()->IsPressed('H'))
		{
			if(fesd)
			{
				if(GetGraphics()->GetCamera()->GetCameraType() == RTS)
				{
					Vector3 fw = GetGraphics()->GetCamera()->GetForward();
					GetGraphics()->ChangeCamera(FPS);
					GetGraphics()->GetCamera()->SetForward(fw);
					//GetGraphics()->ResizeGraphicsEngine(500, 500);
				}
				else
				{
					Vector3 fw = GetGraphics()->GetCamera()->GetForward();
					GetGraphics()->ChangeCamera(RTS);
					GetGraphics()->GetCamera()->SetForward(fw);
					//GetGraphics()->ResizeGraphicsEngine(750, 250);
				}
				//fesd = false;
			}			
		}
		else
			fesd = true;

		if(GetGraphics()->GetKeyListener()->IsPressed('J'))
			GetGraphics()->SetSceneAmbientLight(Vector3(1.0f, 0.0f, 0.0f));


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
			secModel->MoveBy(Vector3(1, 0, 0) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
			secModel->MoveBy(Vector3(-1, 0, 0) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
			secModel->MoveBy(Vector3(0, 0, 1) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
			secModel->MoveBy(Vector3(0, 0, -1) * diff * 0.1f);

		//CASCADED SHADOW MAPPING
		static float debugCSMScale = 1.0f;
		if(GetGraphics()->GetKeyListener()->IsPressed('Q'))
		{
			debugCSMScale += diff * 0.001f;
			model->Scale(1 + diff * 0.01f);
			fileNames[0] = "Media/TerrainTexture.png";
			fileNames[1] = "Media/BallTexture.png";
			fileNames[2] = "Media/BallTexture.png";
			fileNames[3] = "Media/BallTexture.png";
			iT->SetTextures(fileNames);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('E'))
		{
			debugCSMScale -= diff * 0.001f;
			model->Scale(1 - diff * 0.01f);
			fileNames[0] = "Media/BallTexture.png";
			fileNames[1] = "Media/TerrainTexture.png";
			fileNames[2] = "Media/TerrainTexture.png";
			fileNames[3] = "Media/TerrainTexture.png";
			iT->SetTextures(fileNames);
		}
		/*for(int i = 0; i < ttte * nrOfFrustumSlices; i++)
		{
			//debugCSMPoints[i]->SetScale(debugCSMScale);
		}*/
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
		

		/*
		templol += diff;
		if(templol > 100000.0f)
		{
			CollisionData secCD = GetGraphics()->GetPhysicsEngine()->GetCollisionMeshMesh(model, secModel);
			if(secCD.collision)
			{
				secModel->SetSpecialColor(RED_COLOR);
				MaloW::Debug("Collision! :D");
			}
			else
			{
				MaloW::Debug("No collision... :(");
				secModel->SetSpecialColor(WHITE_COLOR);
			}
			templol = 0.0f;
		}
		
		*/
#endif
//*************************************	    END OF RUN TESTS       **********************


		li->SetPosition(GetGraphics()->GetCamera()->GetPosition());

		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			GetGraphics()->GetCamera()->MoveLeft(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
			GetGraphics()->GetCamera()->MoveBackward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
			GetGraphics()->GetCamera()->MoveRight(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed('Z'))	
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				(float)(GetGraphics()->GetEngineParameters()->windowWidth) / 2.0f, 
				(float)(GetGraphics()->GetEngineParameters()->windowHeight) / 2.0f));

			GetGraphics()->GetCamera()->SetUpdateCamera(false);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('X'))	
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				GetGraphics()->GetEngineParameters()->windowWidth / 2.0f, 
				GetGraphics()->GetEngineParameters()->windowHeight / 2.0f));
			GetGraphics()->GetCamera()->SetUpdateCamera(true);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(false);
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('C'))
		{
			if(toggleScale)
			{
				if(showscale)
					scaleHuman->SetPosition(Vector3(30, -100, 30));
				else
					scaleHuman->SetPosition(Vector3(15, 20, 25));
				showscale = !showscale;
				toggleScale = false;
			}
		}
		else
			toggleScale = true;


		if(GetGraphics()->GetKeyListener()->IsPressed('L'))
		{
			if(toggleDelCache)
			{
				deleteCache();
				toggleDelCache = false;
			}
		}
		else
			toggleDelCache = true;


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f + diff * 0.01f));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f - diff * 0.01f));

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SPACE))
			GetGraphics()->GetCamera()->MoveUp(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
			GetGraphics()->GetCamera()->MoveDown(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;


		if(GetGraphics()->GetKeyListener()->IsPressed('1'))
		{
			if(toggleLight)
			{
				if(li->GetIntensity() < 0.01f)
				{
					li->SetIntensity(tempInt);
				}
				else
				{
					tempInt = li->GetIntensity();
					li->SetIntensity(0.001f);
				}
				toggleLight = false;
			}
		}
		else
			toggleLight = true;

		if(GetGraphics()->GetKeyListener()->IsPressed('T'))
		{
			li->SetIntensity(li->GetIntensity() * (1.0f + diff * 0.002f));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('Y'))
		{
			li->SetIntensity(li->GetIntensity() * (1.0f - diff * 0.002f));
		}


		if(GetGraphics()->GetKeyListener()->IsPressed('B'))
		{
			GetGraphics()->SetSceneAmbientLight(GetGraphics()->GetSceneAmbientLight() * (1.0f + diff * 0.002f));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('N'))
		{
			GetGraphics()->SetSceneAmbientLight(GetGraphics()->GetSceneAmbientLight() * (1.0f - diff * 0.002f));
		}

		
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
			GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
			GetGraphics()->SetSunLightProperties(Vector3(-1, -1, -1));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
			GetGraphics()->SetSunLightProperties(Vector3(-1, -1, 1));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
			GetGraphics()->SetSunLightProperties(Vector3(1, -1, -1));
		
	}
	
	FreeGraphics();


	//*************************************	     POST TEST       **********************
#ifdef TEST
	delete[] testData;
	//delete[] debugCSMPoints;
	//delete[] vertices;
	delete[] iTs;
	//delete[] fileNames;
#endif
	//*************************************	   END OF POST TEST       **********************


	return 0;
}
