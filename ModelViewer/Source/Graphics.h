#pragma once

#include <GraphicsEngine.h>
#include "Windows.h"

#ifdef _DEBUG
#pragma comment(lib, "NDYGFXD.lib")
#else
#pragma comment(lib, "NDYGFX.lib")
#endif

bool GraphicsInit( HINSTANCE hInstance );
GraphicsEngine* GetGraphics();
bool FreeGraphics();