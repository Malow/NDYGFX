#include "Graphics.h"
#include "MaloWFileDebug.h"


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
	if ( !GraphicsInit(hInstance) )
		throw("Failed Creating Graphics Engine!");

	MaloW::ClearDebug();
	GetGraphics()->CreateSkyBox("Media/skymap.dds");	// Reduces FPS from 130 to 40
	//iTerrain* t = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(100, 1, 100), "Media/TerrainTexture.png", "Media/TerrainHeightmap.raw");
	GetGraphics()->GetCamera()->SetPosition(Vector3(50, 30, 50));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	//iLight* i = GetGraphics()->CreateLight( Vector3(15.0f, 75.0f, 15.0f) );
	//i->SetIntensity(1000.1f);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
	iTerrain* iT = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(10, 10, 10), 2);


	iMesh* scaleHuman = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	scaleHuman->Scale(1.0f / 20.0f);
	model->Scale(1.0f * 0.05f);

	GetGraphics()->StartRendering();	// To force the engine to render a black image before it has loaded stuff to not get a clear-color rendered before skybox is in etc.

	string lastSpecString = "";
	bool showscale = false;
	bool toggleScale = true;
	bool toggleDelCache = true;
	bool go = true;
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
			//GetGraphics()->DeleteMesh(model);
			//model = GetGraphics()->CreateMesh(loadModel.c_str(), Vector3(15, 20, 20));
			//model->Scale(1.0f / 20.0f);
			lastSpecString = specString;
		}

		// Updates camera etc, does NOT render the frame, another process is doing that, so diff should be very low.
		float diff = GetGraphics()->Update();	

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





		////////////////// MaloW Testing
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

	return 0;
}
