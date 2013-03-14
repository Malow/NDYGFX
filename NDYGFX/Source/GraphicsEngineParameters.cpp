#include "GraphicsEngineParameters.h"
#include "MaloWFileDebug.h"


void GraphicsEngineParams::LoadFromFile(const char* file)
{
	std::ifstream in;
	in.open(file);
	if(!in)
	{
		MaloW::Debug("Failed to load EngineParameters from " + string(file) + ". Might be because it didn't exist, creating it now.");
		this->SaveToFile(file);
		return;
	}

	std::string line = "";

	getline(in, line);
	getline(in, line);
	this->WindowWidth = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->WindowHeight = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->Maximized = atoi(line.c_str()) > 0;
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->ShadowMapSettings = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->FXAAQuality = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->CamType = (CameraType)atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->FOV = (float)atof(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->NearClip = (float)atof(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->FarClip = (float)atof(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->RefreshRate = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->MaxFPS = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->ShadowFit = atoi(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->BillboardRange = atof(line.c_str());
	getline(in, line);
	getline(in, line);
	getline(in, line);
	this->MouseSensativity = atof(line.c_str());

	in.close();
}

void GraphicsEngineParams::SaveToFile(const char* file)
{
	std::ofstream out;
	out.open(file);
	if(!out)
	{
		MaloW::Debug("Failed to save EngineParameters to " + string(file));
		return;
	}

	out << "Window width:		Default: 1024" << endl;
	out << this->WindowWidth << std::endl;
	out << endl;
	out << "Window height:		Default: 768" << endl;
	out << this->WindowHeight << std::endl;
	out << endl;
	out << "Borderless window (0 - 1):		Default: 0" << endl;
	out << this->Maximized << std::endl;
	out << endl;
	out << "Shadowmap Quality (0 - 8 (higher possible)):	Default: 0" << endl;
	out << this->ShadowMapSettings << std::endl;
	out << endl;
	out << "FXAA Quality (0 - 4):	Default: 0" << endl;
	out << this->FXAAQuality << std::endl;
	out << endl;
	out << "Camera Type (0 = FPS, 1 = RTS):		Default: 0" << endl;
	out << this->CamType << std::endl;
	out << endl;
	out << "Field of View:	Default: 75" << endl;
	out << this->FOV << std::endl;
	out << endl;
	out << "NearClip:	Default: 0.2" << endl;
	out << this->NearClip << std::endl;
	out << endl;
	out << "Far Clip:	Default: 200" << endl;
	out << this->FarClip << std::endl;
	out << endl;
	out << "Refresh Rate (hz):		Default: 60" << endl;
	out << this->RefreshRate << std::endl;
	out << endl;
	out << "Max FPS:		Default: 0 (unlimited)" << endl;
	out << this->MaxFPS << std::endl;
	out << endl;
	out << "ShadowFit:		Default: 50" << endl;
	out << this->ShadowFit << std::endl;
	out << endl;
	out << "BillboardRange(%):		Default: 0.75" << endl;
	out << this->BillboardRange << std::endl;
	out << endl;
	out << "Mouse Sensativity:		Default: 1.0" << endl;
	out << this->MouseSensativity << std::endl;
	out << endl;

	out.close();
}

/*
void GraphicsEngineParams::SaveToFile(const char* file, int winWidth, int winHeight, int winMax, int shadowMapQual, int FXAAQual )
{
std::ofstream out;
out.open(file);
if(!out)
{
MaloW::Debug("Failed to save EngineParameters from " + string(file));
return;
}

out << winWidth << std::endl;
out << winHeight << std::endl;
out << winMax << std::endl;
out << shadowMapQual << std::endl;
out << FXAAQual << std::endl;

out.close();
}
*/