#pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iStaticMesh.h"

extern "C"
{
	class DECLDIR GraphicsEngine
	{
	public:
		GraphicsEngine() { };
		virtual ~GraphicsEngine() { };

		static GraphicsEngine* eng;

		iStaticMesh* CreateStaticMesh(const char* filename, Vector3 pos);
	};

	

	/*
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, MaterialType material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, Material* material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos);
	void DeleteStaticMesh(StaticMesh* mesh) { this->dx->DeleteStaticMesh(mesh); }

	AnimatedMesh* CreateAnimatedMesh(string filename, D3DXVECTOR3 pos);
	void DeleteAnimatedMesh(AnimatedMesh* mesh) { this->dx->DeleteAnimatedMesh(mesh); }

	Light* CreateLight(D3DXVECTOR3 pos, bool UseShadowMap = true);
	void DeleteLight(Light* light) { this->dx->DeleteLight(light); }

	Terrain* CreateTerrain(D3DXVECTOR3 position, D3DXVECTOR3 dimension, std::string texture, string heightmap, int vertexSize = 256);

	void CreateSkyBox(string texture);

	Image* CreateImage(D3DXVECTOR2 position, D3DXVECTOR2 dimensions, string texture);
	bool DeleteImage(Image* delImage);

	Text* CreateText(string text, D3DXVECTOR2 position, float size, string fontTexturePath);
	bool DeleteText(Text* delText);

	GraphicsEngineParams GetEngineParameters() const { return this->parameters; }

	float Update();

	void LoadingScreen(string BackgroundTexture = "", string ProgressBarTexture = "", float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, float FadeBlackOutOutTime = 0.0f);

	bool isRunning();
	void StartRendering() { this->dx->StartRender = true; }

	Camera* GetCamera() const { return this->cam; } //{ return this->dx->GetCamera(); }
	MaloW::KeyListener* GetKeyListener() const { return this->kl; }
	HWND GetWindowHandle() const { return this->hWnd; }


	EXTRAS:
	SetParamater - To allow change of parameters after creation


	WRAPPER:
	StaticMesh*
	AnimatedMesh*
	Light*
	Terrain*
	Image*
	Text*
	Camera*
	MaloW::KeyListener*
	GraphicsEngineParams		--- Done
	*/

	DECLDIR GraphicsEngine* CreateGraphicsEngineInWindow(unsigned int hWnd, const char* configFile);
	DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile);
	DECLDIR GraphicsEngine* GetGraphicsEngine();
}