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
	this->windowWidth = atoi(line.c_str());
	getline(in, line);
	this->windowHeight = atoi(line.c_str());
	getline(in, line);
	this->Maximized = (bool)atoi(line.c_str());
	getline(in, line);
	this->ShadowMapSettings = atoi(line.c_str());
	getline(in, line);
	this->FXAAQuality = atoi(line.c_str());
	getline(in, line);
	this->CamType = (CameraType)atoi(line.c_str());
	getline(in, line);
	this->FOV = atof(line.c_str());
	getline(in, line);
	this->NearClip = atof(line.c_str());
	getline(in, line);
	this->FarClip = atof(line.c_str());
	getline(in, line);
	this->RefreshRate = atoi(line.c_str());

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

	out << this->windowWidth << std::endl;
	out << this->windowHeight << std::endl;
	out << this->Maximized << std::endl;
	out << this->ShadowMapSettings << std::endl;
	out << this->FXAAQuality << std::endl;
	out << this->CamType << std::endl;
	out << this->FOV << std::endl;
	out << this->NearClip << std::endl;
	out << this->FarClip << std::endl;
	out << this->RefreshRate << std::endl;

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