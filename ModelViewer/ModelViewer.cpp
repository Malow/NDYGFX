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

	while(true)
	{
		if(gfx->GetKeyListener()->IsPressed('W'))
		{
			gfx->GetCamera()->MoveForward(0.001f);
		}
		gfx->Update();
		Sleep(50);
	}
	
	delete gfx;

	return 0;
}
