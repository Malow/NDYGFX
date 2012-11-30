#pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"
#include "iLight.h"
#include "iImage.h"
#include "iText.h"
#include "iAnimatedMesh.h"
#include "iCamera.h"
#include "iTerrain.h"
#include "iKeyListener.h"
#include "iTerrain.h"
#include "iGraphicsEngineParams.h"

extern "C"
{
	class DECLDIR GraphicsEngine
	{
	public:
		GraphicsEngine() { };
		virtual ~GraphicsEngine() { };

		virtual iMesh* CreateMesh(const char* filename, const Vector3& pos) = 0;
		virtual void DeleteMesh(iMesh* delMesh) = 0;

		virtual iLight* CreateLight(Vector3 pos) = 0;
		virtual void DeleteLight(iLight* light) = 0;

		virtual iImage* CreateImage(Vector2 pos, Vector2 dimensions, const char* texture) = 0;
		virtual void DeleteImage(iImage* delImg) = 0;

		virtual iText* CreateText(const char* text, Vector2 pos, float size, const char* fontTexturePath) = 0;
		virtual void DeleteText(iText* deltxt) = 0;

		virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos) = 0;
		virtual iAnimatedMesh* CreateAnimatedMesh(const char* filename, const Vector3& pos) = 0;
		virtual iTerrain* CreateTerrain(const Vector3& pos, const Vector3& dimensions, const char* texture, const char* heightMap, unsigned int vertexSize = 256) = 0;

		virtual iCamera* GetCamera() const = 0;

		virtual iKeyListener* GetKeyListener() const = 0;

		/*! Updates the Camera and takes care of all key-inputs and returns diff in seconds (ie. 0.000065f seconds) */
		virtual float Update() = 0;

		virtual bool IsRunning() = 0;

		virtual iGraphicsEngineParams* GetEngineParameters() const = 0;

		virtual void CreateSkyBox(const char* texture) = 0;

		virtual void StartRendering() = 0;

		virtual const char* GetSpecialString() = 0;

		virtual void SetFPSMax(float maxFPS) = 0;
	};

	/*
	// Maybe ToDo in future:
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, MaterialType material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, Material* material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos);
	void DeleteStaticMesh(StaticMesh* mesh) { this->dx->DeleteStaticMesh(mesh); }
	AnimatedMesh* CreateAnimatedMesh(string filename, D3DXVECTOR3 pos);
	void DeleteAnimatedMesh(AnimatedMesh* mesh) { this->dx->DeleteAnimatedMesh(mesh); }


	// To do:
	void LoadingScreen(string BackgroundTexture = "", string ProgressBarTexture = "", float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, float FadeBlackOutOutTime = 0.0f);
	HWND GetWindowHandle() const { return this->hWnd; }
	*/

	DECLDIR GraphicsEngine* CreateGraphicsEngineInWindow(unsigned int hWnd, const char* configFile);
	DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile);
}