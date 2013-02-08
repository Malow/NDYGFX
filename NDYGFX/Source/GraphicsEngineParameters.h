#ifndef MALOWENGINEPARAMS
#define MALOWENGINEPARAMS

#include <fstream>
#include <string>
#include "iGraphicsEngineParams.h"

class GraphicsEngineParams : public iGraphicsEngineParams
{
public:
	GraphicsEngineParams() {}
	virtual ~GraphicsEngineParams() {}

	void LoadFromFile(const char* file);
	virtual void SaveToFile(const char* file);
	//void SaveToFile(const char* file, int winWidth, int winHeight, int winMax, int shadowMapQual, int FXAAQual);
};

#endif