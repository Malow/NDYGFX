#include "GraphicsEngine.h"
#include "GraphicsEngineImp.h"


DECLDIR GraphicsEngine* CreateGraphicsEngineInWindow(unsigned int hWnd, const char* configFile)
{
	GraphicsEngineParams GEP;
	GEP.LoadFromFile(configFile);
	GraphicsEngine* eng = new GraphicsEngineImp(GEP, (HWND)hWnd);	/// TO DO CHANGE THIS SO THAT IT WORKS WITH C#
	return eng;
}

DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile)
{
	GraphicsEngineParams GEP;
	GEP.LoadFromFile(configFile);
	GraphicsEngine* eng = new GraphicsEngineImp(GEP, (HINSTANCE)hInstance, 1);
	return eng;
}