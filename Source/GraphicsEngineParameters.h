#ifndef MALOWENGINEPARAMS
#define MALOWENGINEPARAMS

#include <fstream>
#include <string>



enum CameraType
{
	FPS = 0,
	RTS = 1,
	TRD = 2,
};

class GraphicsEngineParams
{
public:
	static int windowWidth;
	static int windowHeight;
	static bool Maximized;
	static int ShadowMapSettings;
	static int FXAAQuality;
	static CameraType CamType;

	GraphicsEngineParams() {}
	virtual ~GraphicsEngineParams() {}


	void LoadFromeFile(const char* file);
	void SaveToFile(const char* file);
	void SaveToFile(const char* file, int winWidth, int winHeight, int winMax, int shadowMapQual, int FXAAQual );


	
};

#endif