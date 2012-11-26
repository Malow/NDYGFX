#include <windows.h>
#include <GraphicsEngine.h>

#ifdef _DEBUG
#pragma comment(lib, "NDYGFXD.lib")
#else
#pragma comment(lib, "NDYGFX.lib")
#endif



int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	GraphicsEngine* gfx = CreateGraphicsEngine((unsigned int)hInstance, "config.cfg");
	//gfx->CreateSkyBox("Media/skymap.dds");	// Reduces FPS from 130 to 40

	gfx->CreateTerrain(Vector3(0, 0, 0), Vector3(100, 1, 100), "Media/TerrainTexture.png", "Media/TerrainHeightmap.raw");

	gfx->StartRendering();	// To force the engine to render a black image before it has loaded stuff to not get a clear-color rendered before skybox is in etc.
	bool go = true;
	while(go)	// Returns true as long as ESC hasnt been pressed, if it's pressed the game engine will shut down itself (to be changed)
	{
		Sleep(1);
		float diff = gfx->Update();	// Updates camera etc, does NOT render the frame, another process is doing that, so diff should be very low.

		if(gfx->GetKeyListener()->IsPressed('W'))
			gfx->GetCamera()->MoveForward(diff);
		if(gfx->GetKeyListener()->IsPressed('A'))
			gfx->GetCamera()->MoveLeft(diff);
		if(gfx->GetKeyListener()->IsPressed('S'))	
			gfx->GetCamera()->MoveBackward(diff);
		if(gfx->GetKeyListener()->IsPressed('D'))	
			gfx->GetCamera()->MoveRight(diff);

		if(gfx->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;
	}
	
	delete gfx;

	return 0;
}
