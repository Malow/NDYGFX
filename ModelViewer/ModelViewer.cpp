#include "Graphics.h"
#include "MaloWFileDebug.h"

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	if ( !GraphicsInit(hInstance) )
		throw("Failed Creating Graphics Engine!");

	MaloW::ClearDebug();
	GetGraphics()->CreateSkyBox("Media/skymap.dds");	// Reduces FPS from 130 to 40
	iTerrain* t = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(100, 1, 100), "Media/TerrainTexture.png", "Media/TerrainHeightmap.raw");
	
	iLight* i = GetGraphics()->CreateLight( Vector3(0.0f, t->getYPositionAt(0.0f,0.0f)+10.0f, 0.0f) );
	i->SetIntensity(1000.0f);
	
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 15, 15));

	GetGraphics()->StartRendering();	// To force the engine to render a black image before it has loaded stuff to not get a clear-color rendered before skybox is in etc.
	
	string lastLoadModel = "";
	bool go = true;
	while(GetGraphics()->IsRunning() && go)
	{
		Sleep(10);
		string loadModel = GetGraphics()->GetSpecialString();
		if(loadModel != lastLoadModel)
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			string path = string(szPath);
			path = path.substr(0, path.size() - string("ModelViewer.exe").size());


			loadModel = loadModel.substr(path.size(), loadModel.size());
			MaloW::Debug(path);
			GetGraphics()->DeleteMesh(model);
			model = GetGraphics()->CreateMesh(loadModel.c_str(), Vector3(15, 15, 15));
			lastLoadModel = loadModel;
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
		if (GetGraphics()->GetKeyListener()->IsPressed('C'))
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				GetGraphics()->GetEngineParameters()->windowWidth / 2, 
				GetGraphics()->GetEngineParameters()->windowHeight / 2));
		}


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;
	}
	
	FreeGraphics();

	return 0;
}
