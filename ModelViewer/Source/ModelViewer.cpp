#define TEST //<----------------------- kommentera ut vid behov **********************


#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
	#define INCLUDE_MODEL_VIEWER
#endif

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"
#include "TestMaloW.h"
#include "TestTillman.h"
#include "TestOther.h"

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

	GetGraphics()->CreateSkyBox("Media/skymap.dds"); //** TILLMAN
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	iLight* li = GetGraphics()->CreateLight(GetGraphics()->GetCamera()->GetPosition());
	li->SetIntensity(0.001f);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	iMesh* scaleHuman = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	scaleHuman->Scale(1.0f / 20.0f);
	model->Scale(1.0f * 0.05f);

//*************************************	     PRE TEST       **********************
#ifdef TEST
	MaloWTest mt;
	TillmanTest tt;
	OtherTest ot;
	mt.PreTest();
	tt.PreTest();
	ot.PreTest();
	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png"
		, 1.0f, 1.0f, 1.0f, 1.0f);
#endif
//*************************************	    END OF PRE TEST       **********************

	// To stop the engine rendering a splash image before it has loaded stuff 
	// to not get a clear-color rendered before skybox is in etc.
	GetGraphics()->StartRendering();	

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
	mt.RunTest(diff);
	tt.RunTest(diff);
	ot.RunTest(diff);
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
				(float)(GetGraphics()->GetEngineParameters()->WindowWidth) / 2.0f, 
				(float)(GetGraphics()->GetEngineParameters()->WindowHeight) / 2.0f));

			GetGraphics()->GetCamera()->SetUpdateCamera(false);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('X'))	
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				GetGraphics()->GetEngineParameters()->WindowWidth / 2.0f, 
				GetGraphics()->GetEngineParameters()->WindowHeight / 2.0f));
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
	

	//*************************************	     POST TEST       **********************
#ifdef TEST
	mt.PostTest();
	tt.PostTest();
	ot.PostTest();
#endif
	//*************************************	   END OF POST TEST       **********************

	FreeGraphics();
	return 0;
}
