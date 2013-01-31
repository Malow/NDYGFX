#ifndef MALOWENGINEPARAMS
#define MALOWENGINEPARAMS

#include <fstream>
#include <string>
#include "iGraphicsEngineParams.h"

class GraphicsEngineParams : public iGraphicsEngineParams
{
public:
	GraphicsEngineParams()
	{
		WindowWidth = 1024;
		WindowHeight = 768;
		Maximized = false;
		ShadowMapSettings = 0;
		FXAAQuality = 0;
		CamType = CameraType::FPS;
		FOV = 0.45f;
		NearClip = 0.01f;
		FarClip = 200.0f;
		RefreshRate = 60;
		MaxFPS = 0;
		ShadowFit = 50;
	};

	virtual ~GraphicsEngineParams() {}

	virtual void LoadFromFile(const char* file);
	virtual void SaveToFile(const char* file);
};

#endif