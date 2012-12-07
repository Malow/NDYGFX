#include "Graphics.h"
#include "MaloWFileDebug.h"

//#define TEST //<----------------------- kommentera ut vid behov **********************

void ReplaceSlashes(string& str, char replace, char with)
{
	for(int i = 0; i < str.size(); i++)
	{
		if(str.at(i) == replace)
			str.at(i) = with;
	}
}

void deleteCache()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	string path = string(szPath);
	path = path.substr(0, path.size() - string("ModelViewer.exe").size());
	path += "\Media\\Cache";
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

	int ret = SHFileOperation(&file_op);
	free(tempdir);
}

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	MaloW::ClearDebug();

	if ( !GraphicsInit(hInstance) )
		throw("Failed Creating Graphics Engine!");
	
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
	iTerrain* iT = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(10, 10, 10), vertSize);
	

	float* hmData = new float[vertSize * vertSize];
	//for(int i = 0; i < vertSize; i++)
	{
		hmData[0] = 0.5f;
		hmData[1] = 0.0f;
		hmData[2] = 0.0f;
		hmData[3] = 0.0f;
	}
	iT->SetHeightMap(hmData);
	const char** fileNames = new const char*[3];
	fileNames[0] = "Media/TerrainTexture.png";
	fileNames[1] = "Media/BallTexture.png";
	fileNames[2] = "Media/TerrainTexture.png";
	iT->SetTextures(fileNames);
	
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
			testData[i] = 0.0f; //B
		}
		else if(i % 4 == 3)
		{
			testData[i] = 0.0f; //A
		}
	}
	iT->SetBlendMap(size, testData);
	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, -100, 0));
	ball->Scale(0.1f);
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
			GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->GetForward(), model);
		if(cd.collision)
		{
			ball->SetPosition(cd.position);
		}
		else
		{
			ball->SetPosition(Vector3(0, -100, 0));
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('H'))
			GetGraphics()->ChangeCamera(RTS);

		if(GetGraphics()->GetKeyListener()->IsPressed('J'))
			GetGraphics()->SetSceneAmbientLight(Vector3(1.0f, 0.0f, 0.0f));

		static bool asd = true;
		if(GetGraphics()->GetKeyListener()->IsPressed('K'))
			if(asd)
			{
				GetGraphics()->ResizeGraphicsEngine(500, 500);
				asd = false;
			}
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
				GetGraphics()->GetEngineParameters()->windowWidth / 2, 
				GetGraphics()->GetEngineParameters()->windowHeight / 2));
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
	delete[] hmData;
	delete[] fileNames;
	//delete[] testData;
#endif
	//*************************************	   END OF POST TEST       **********************
	return 0;
}
