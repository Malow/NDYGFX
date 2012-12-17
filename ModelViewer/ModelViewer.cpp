#define TEST //<----------------------- kommentera ut vid behov **********************


#include "Graphics.h"
#include "MaloWFileDebug.h"

void ReplaceSlashes(string& str, char replace, char with)
{
	for(unsigned int i = 0; i < str.size(); i++)
	{
		if(str.at(i) == replace)
			str.at(i) = with;
	}
}

// TODO: does not work
void deleteCache()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	string path = string(szPath);
	path = path.substr(0, path.size() - string("ModelViewer.exe").size());
	path += "Media\\Cache";
	MaloW::Debug(path);

	int len = strlen(path.c_str()) + 2; // required to set 2 nulls at end of argument to SHFileOperation.
	char* tempdir = (char*) malloc(len);
	memset(tempdir,0,len);
	strcpy(tempdir,path.c_str());

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

	SHFileOperation(&file_op);

	free(tempdir);
}

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	MaloW::ClearDebug();

	if ( !GraphicsInit(hInstance) )
		MaloW::Debug("Failed Creating Graphics Engine!");
	
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	GetGraphics()->CreateSkyBox("Media/skymap.dds");	// Reduces FPS from 130 to 40
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	iLight* i = GetGraphics()->CreateLight(GetGraphics()->GetCamera()->GetPosition());
	i->SetIntensity(0.001f);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	
//*************************************	     PRE TEST       **********************
#ifdef TEST
	int vertSize = 2;
	iTerrain* iT = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(100.0f, 0.0f, 100.0f), vertSize);
	//iAnimatedMesh* iAM = GetGraphics()->CreateAnimatedMesh("Media/TestMedia/FlagBlue.ani", Vector3(0, 0, 0));
	iImage* iM = GetGraphics()->CreateImage(Vector2(100, 100), Vector2(100, 100), "Media/BallTexture.png");
	iText* iTe = GetGraphics()->CreateText("durp", Vector2(300, 100), 1.0f, "Media/TestMedia/1");
	
	float test = 0.0f;
	Vector2 dd = Vector2(0.1f, 0.2f);
	test = dd[0];
	Vector3 durp = Vector3(1.0f, 2.0f, 3.0f);
	test = durp[2];
	Vector4 hurp = Vector4(10.0f, 20.0f, 30.0f);
	test = hurp[3];

	//D3DXVECTOR2 ddd = dd.GetD3DXVECTOR2(); //**tillman / malow **

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
	fileNames[1] = "Media/BallTexture.png";
	fileNames[2] = "Media/BallTexture.png";
	fileNames[3] = "Media/TerrainTexture.png";
	iT->SetTextures(fileNames);
	iT->SetTextureScale(-4);
	
	int size = 2048*2048*4;
	float* testData = new float[size]; 
	for(int i = 0; i < size; i++)
	{
		if(i % 4 == 0)
		{
			testData[i] = 1.0f; //R
		}
		else if(i % 4 == 1)
		{
			testData[i] = 1.0f; //G
		}
		else if(i % 4 == 2)
		{
			testData[i] = 1.0f; //B
		}
		else if(i % 4 == 3)
		{
			testData[i] = 1.0f; //A
		}
	}
	iT->SetBlendMap(size, testData);
	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, -100, 0));
	ball->Scale(0.1f);
	iMesh* secModel = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(10, 0, 10));
	secModel->Scale(1.0f * 0.05f);

	float templol = 0.0f;
	
#endif
//*************************************	    END OF PRE TEST       **********************

	

	iMesh* scaleHuman = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	scaleHuman->Scale(1.0f / 20.0f);
	model->Scale(1.0f * 0.05f);

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
			model = GetGraphics()->CreateMesh(loadModel.c_str(), Vector3(15, 20, 20));
			model->Scale(1.0f / 20.0f);
			lastSpecString = specString;
		}

		// Updates camera etc, does NOT render the frame, another process is doing that, so diff should be very low.
		float diff = GetGraphics()->Update();	
		
//*************************************	     RUN TESTS       **********************
#ifdef TEST
		CollisionData cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
			GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), model);
		if(cd.collision)
		{
			ball->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
		}
		else
		{
			ball->SetPosition(Vector3(0, -100, 0));
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
				}
				else
				{
					Vector3 fw = GetGraphics()->GetCamera()->GetForward();
					GetGraphics()->ChangeCamera(RTS);
					GetGraphics()->GetCamera()->SetForward(fw);
				}
				fesd = false;
			}			
		}
		else
			fesd = true;

		if(GetGraphics()->GetKeyListener()->IsPressed('J'))
			GetGraphics()->SetSceneAmbientLight(Vector3(1.0f, 0.0f, 0.0f));

		static bool asd = true;
		if(GetGraphics()->GetKeyListener()->IsPressed('K'))
		{
			if(asd)
			{
				GetGraphics()->ResizeGraphicsEngine(500, 500);
				asd = false;
			}
		}


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
			secModel->MoveBy(Vector3(1, 0, 0) * diff * 0.01f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
			secModel->MoveBy(Vector3(-1, 0, 0) * diff * 0.01f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
			secModel->MoveBy(Vector3(0, 0, 1) * diff * 0.01f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
			secModel->MoveBy(Vector3(0, 0, -1) * diff * 0.01f);


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
		i->SetPosition(GetGraphics()->GetCamera()->GetPosition());

		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			GetGraphics()->GetCamera()->MoveForward(diff);
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			GetGraphics()->GetCamera()->MoveLeft(diff);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
			GetGraphics()->GetCamera()->MoveBackward(diff);
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
			GetGraphics()->GetCamera()->MoveRight(diff);

		if(GetGraphics()->GetKeyListener()->IsPressed('Z'))	
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				GetGraphics()->GetEngineParameters()->windowWidth / 2, 
				GetGraphics()->GetEngineParameters()->windowHeight / 2));
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


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;


		if(GetGraphics()->GetKeyListener()->IsPressed('1'))
		{
			if(toggleLight)
			{
				if(i->GetIntensity() < 0.01f)
				{
					i->SetIntensity(tempInt);
				}
				else
				{
					tempInt = i->GetIntensity();
					i->SetIntensity(0.001f);
				}
				toggleLight = false;
			}
		}
		else
			toggleLight = true;

		if(GetGraphics()->GetKeyListener()->IsPressed('T'))
		{
			i->SetIntensity(i->GetIntensity() * (1.0f + diff * 0.002f));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('Y'))
		{
			i->SetIntensity(i->GetIntensity() * (1.0f - diff * 0.002f));
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
	//delete[] fileNames;
#endif
	//*************************************	   END OF POST TEST       **********************
	return 0;
}
