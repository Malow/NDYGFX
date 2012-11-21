#include "GraphicsEngine.h"
#include "GraphicsEngineImp.h"

GraphicsEngine* GraphicsEngine::eng = NULL;

DECLDIR GraphicsEngine* CreateGraphicsEngineInWindow(unsigned int hWnd, const char* configFile)
{
	GraphicsEngineParams GEP;
	GEP.LoadFromeFile(configFile);
	GraphicsEngine::eng = new GraphicsEngineImp(GEP, 0, 1);	/// TO DO CHANGE THIS SO THAT IT WORKS WITH C#
	return GraphicsEngine::eng;
}

DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile)
{
	GraphicsEngineParams GEP;
	GEP.LoadFromeFile(configFile);
	GraphicsEngine::eng = new GraphicsEngineImp(GEP, (HINSTANCE)hInstance, 1);
	return GraphicsEngine::eng;
}

DECLDIR GraphicsEngine* GetGraphicsEngine()
{
	return GraphicsEngine::eng;
}

iStaticMesh* GraphicsEngine::CreateStaticMesh(const char* filename, Vector3 pos)
{
	return ((GraphicsEngineImp*)eng)->CreateStaticMesh(string(filename), D3DXVECTOR3(pos.x, pos.y, pos.z));
}