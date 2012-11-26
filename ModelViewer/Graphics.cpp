#include "Graphics.h"

namespace {
	static GraphicsEngine* engine = 0;
}

bool GraphicsInit( HINSTANCE hInstance )
{
	engine = CreateGraphicsEngine((unsigned int)hInstance, "Config.cfg");
	return (engine != 0);
}

GraphicsEngine* GetGraphics()
{
	if ( !engine )
		throw("Engine Not Initialized!");

	return engine;
}

bool FreeGraphics()
{
	if ( !engine )
		return 0;

	delete engine;

	return true;
}