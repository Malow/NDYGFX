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
#include "iBillboard.h"
#include "iBillboardCollection.h"
#include "iText.h"
#include "iAnimatedMesh.h"
#include "iCamera.h"
#include "iTerrain.h"
#include "iKeyListener.h"
#include "iTerrain.h"
#include "iGraphicsEngineParams.h"
#include "iPhysicsEngine.h"
#include "iWaterPlane.h"
#include "iFBXMesh.h"
#include "iDecal.h"

extern "C"
{
	class DECLDIR GraphicsEngine
	{
	public:
		GraphicsEngine() { };
		virtual ~GraphicsEngine() { };

		virtual iMesh* CreateMesh(const char* filename, const Vector3& pos, const char* billboardFilePath = "", float distanceToSwapToBillboard = -1) = 0;
		virtual void DeleteMesh(iMesh* delMesh) = 0;
		virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos) = 0;
		virtual iAnimatedMesh* CreateAnimatedMesh(const char* filename, const Vector3& pos) = 0;

		virtual iLight* CreateLight(Vector3 pos) = 0;
		virtual void DeleteLight(iLight* &light) = 0;

		virtual iImage* CreateImage(Vector2 pos, Vector2 dimensions, const char* texture) = 0;
		virtual void DeleteImage(iImage* &delImg) = 0;

		virtual iBillboard* CreateBillboard(Vector3 pos, Vector2 size, Vector3 color, const char* texture) = 0;
		virtual void DeleteBillboard(iBillboard* &delBillboard) = 0;

		virtual iBillboardCollection* CreateBillboardCollection(unsigned int nrOfVertices, Vector3* positions, 
																Vector2* sizes, Vector3* colors, Vector3& offsetVector, 
																const char* texture ) = 0;
		virtual void DeleteBillboardCollection(iBillboardCollection* &delBillboardCollection) = 0;


		/*! fontTexturePath shall not contain the file type. */
		virtual iText* CreateText(const char* text, Vector2 pos, float size, const char* fontTexturePath) = 0;
		virtual void DeleteText(iText* &deltxt) = 0;

		virtual iTerrain* CreateTerrain(const Vector3& pos, const Vector3& scale, const unsigned int& size) = 0;
		virtual void DeleteTerrain( iTerrain*& terrain ) = 0;

		virtual iFBXMesh* CreateFBXMesh(const char* filename, Vector3 pos) = 0;
		virtual void DeleteFBXMesh(iFBXMesh* mesh) = 0;

		virtual iDecal* CreateDecal(Vector3 pos, const char* texture, Vector3 direction, Vector3 up) = 0;
		virtual void DeleteDecal(iDecal* decal) = 0;

		virtual iCamera* GetCamera() const = 0;

		virtual iKeyListener* GetKeyListener() const = 0;

		/*! Updates the Camera and takes care of all key-inputs and returns diff in seconds (ie. 0.000065f seconds) */
		virtual float Update() = 0;

		virtual bool IsRunning() = 0;

		virtual iGraphicsEngineParams& GetEngineParameters() = 0;

		virtual void ChangeSkyBox(const char* texture) = 0;

		virtual void UseShadow(bool useShadow) = 0;

		
		// Must be set if toggling grass on/off is to work.
		virtual void SetGrassFilePath(const char* filePath) = 0;
		// Grass file path must have been set for this to work.
		virtual void RenderGrass(bool flag) = 0;
		virtual bool GetRenderGrassFlag() const = 0;

		/*! OBS! InnerRadius has to be greater than zero. */
		virtual void SetSpecialCircle(float innerRadius, float outerRadius, Vector2& targetPos) const = 0;
		virtual void ResetPerfLogging() = 0;
		virtual void PrintPerfLogging() = 0;

		//Resource manager
		/*
			Texture resources are created with the default format (CreateTextureResourceFromFile(..)).
			Buffer resources are not supported.
			Supported resources are object data resources(.obj & .ani), Texture resources(default format)(.png && .dds).
		*/
		virtual void PreLoadResources(unsigned int nrOfResources, const char** resourcesFileNames) = 0;

		virtual void StartRendering() = 0;

		virtual const char* GetSpecialString() = 0;

		virtual void SetFPSMax(float maxFPS) = 0;

		virtual void SetSunLightProperties(Vector3 direction, Vector3 lightColor = Vector3(1.0f, 1.0f, 1.0f), float intensity = 1.0f) = 0;
		virtual Vector3 GetSunLightDirection() const = 0;
		virtual Vector3 GetSunLightColor() const = 0;
		virtual float GetSunLightIntensity() const = 0;
		virtual void SetSunLightDisabled() = 0;

		virtual iPhysicsEngine* GetPhysicsEngine() const = 0;

		virtual iCamera* ChangeCamera(CameraType newCamType) = 0;

		virtual void SetSceneAmbientLight(Vector3 ambientLight) = 0;
		virtual Vector3 GetSceneAmbientLight() const = 0;

		virtual void ResizeGraphicsEngine(unsigned int width, unsigned int height) = 0;

		virtual void UseSeperateLoadingThread(bool use) = 0;

		virtual void LoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", 
			float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, 
			float FadeBlackOutOutTime = 0.0f) = 0;

		virtual void ShowLoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", 
			float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f) = 0;

		virtual void HideLoadingScreen() = 0;

		virtual iWaterPlane* CreateWaterPlane(Vector3& pos, const char* texture) = 0;
		virtual void DeleteWaterPlane(iWaterPlane* del) = 0;

		virtual void ChangeShadowQuality(int newQual) = 0;
		virtual void ReloadShaders(int shaderIndex) = 0;

		/*
		Adds an enclosing fog effect.
		Center is the center from which the fog is calculated in a circle around.
		Radius is how far out from center to start fading to fog.
		FogFadeFactor is over how much of Radius the fog should fade from nothing to fully solid.
		*/
		virtual void SetEnclosingFogEffect(Vector3 center, float radius, float fogfadefactor = 0.1f) = 0;
		virtual void SetFogColor(Vector3 color) = 0;
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
	
	HWND GetWindowHandle() const { return this->hWnd; }
	*/

	DECLDIR GraphicsEngine* CreateGraphicsEngineInWindow(unsigned int hWnd, const char* configFile);
	DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile);
}