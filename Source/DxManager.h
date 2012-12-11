#ifndef DXMANAGER_H
#define DXMANAGER_H

#include "DirectX.h"
#include "Camera.h"
#include "Shader.h"
#include "Buffer.h"
#include "Array.h"
#include "Mesh.h"
#include "Object3D.h"
#include "GraphicsEngineParameters.h"
#include "Light.h"
#include "Process.h"
#include "Image.h"
#include "SSAO.h"
#include "Text.h"
#include "SkyBox.h"
#include "FXAA.h"
#include "StaticMesh.h"
#include "AnimatedMesh.h"
#include "DxManagerEvents.h"
#include "CascadedShadowMap.h"

class DxManager : public MaloW::Process
{
private:
	// Standard stuff
	ID3D11DeviceContext* Dx_DeviceContext;
	ID3D11DepthStencilView* Dx_DepthStencilView;
	ID3D11Texture2D* Dx_DepthStencil;
	ID3D11RenderTargetView* Dx_RenderTargetView;
	IDXGISwapChain* Dx_SwapChain;
	ID3D11Device* Dx_Device;
	Shader*	Shader_ForwardRendering;
	HWND hWnd;
	D3D11_VIEWPORT Dx_Viewport;

	//Own systems
	long framecount;
	GraphicsEngineParams params;
	Camera* camera;
	MaloW::Array<Terrain*> terrains; //terrain = sector data
	MaloW::Array<StaticMesh*> objects;
	MaloW::Array<AnimatedMesh*> animations;

	MaloW::Array<Image*> images;
	MaloW::Array<Text*> texts;

	float LavaWavesOuterRadius;
	SkyBox* skybox;
	Shader* Shader_Skybox;

	// Lightning
	MaloW::Array<Light*> lights;
	D3DXVECTOR3 sceneAmbientLight;
	SunLight sun;
	bool useSun;

	// Shadow map:
	Shader* Shader_ShadowMap;
	Shader* Shader_BillBoard;
	Shader* Shader_Text;
	Shader* Shader_ShadowMapAnimated;

	CascadedShadowMap* csm;

	// Deferred Rendering
	// Gbuffer:
	static const int NrOfRenderTargets = 4;
	ID3D11Texture2D* Dx_GbufferTextures[NrOfRenderTargets];
	ID3D11RenderTargetView* Dx_GbufferRTs[NrOfRenderTargets];
	ID3D11ShaderResourceView* Dx_GbufferSRVs[NrOfRenderTargets];
	Shader* Shader_DeferredGeometry;
	Shader* Shader_DeferredGeometryBlendMap;
	Shader* Shader_DeferredLightning;
	Shader* Shader_InvisibilityEffect;

	Shader* Shader_DeferredQuad;
	Shader* Shader_DeferredTexture;
	ID3D11Texture2D* Dx_DeferredTexture;
	ID3D11RenderTargetView* Dx_DeferredQuadRT;
	ID3D11ShaderResourceView* Dx_DeferredSRV;

	Shader* Shader_DeferredAnimatedGeometry;


	SSAO* ssao;
	FXAA* fxaa;
	Shader* Shader_Fxaa;

	bool invisibleGeometry;

	// Timer
	float PCFreq;
	__int64 prevTimeStamp;

	float TimerAnimation;
	float LastCamUpdate;
	float RendererSleep;

	void PreRender(); //stdafx.fx
	void RenderForward();
	void RenderDeferredGeometry();
	void RenderDeferredPerPixel();
	void RenderInvisibilityEffect();
	void RenderParticles();
	void RenderShadowMap();
	void RenderCascadedShadowMap();
	void RenderImages();
	void RenderQuadDeferred();
	void RenderDeferredTexture();
	void RenderDeferredSkybox();
	void RenderAntiAliasing();
	void RenderText();

	void ResizeRenderer(ResizeEvent* ev);

	HRESULT Init();

	int TriangleCount;


public:
	bool StartRender;
	DxManager(HWND g_hWnd, GraphicsEngineParams params, Camera* cam);
	virtual ~DxManager();

	void HandleTerrainEvent(TerrainEvent* me);
	void HandleMeshEvent(MeshEvent* me);
	void HandleLightEvent(LightEvent* le);
	void HandleImageEvent(ImageEvent* ie);
	void HandleTextEvent(TextEvent* te);
	virtual void Life();
	HRESULT Render();
	HRESULT Update(float deltaTime);

	void CreateSmokeEffect();
	void CreateTerrain(Terrain* terrain); 
	void CreateStaticMesh(StaticMesh* mesh);
	void CreateAnimatedMesh(AnimatedMesh* mesh);
	Object3D* createParticleObject(ParticleMesh* mesh);
	Light* CreateLight(D3DXVECTOR3 pos, bool UseShadowMap);
	void CreateImage(Image* image, string texture);
	void CreateText(Text* text, string font);
	void CreateSkyBox(string texture);

	long GetFrameCount() const { return this->framecount; }

	void DeleteTerrain(Terrain* terrain);
	void DeleteStaticMesh(StaticMesh* mesh);
	void DeleteAnimatedMesh(AnimatedMesh* mesh);
	void DeleteLight(Light* light);
	void DeleteImage(Image* image);
	void DeleteText(Text* text);

	void SetCamera(Camera* cam) { this->camera = cam; }
	Camera* GetCamera() const { return this->camera; }

	int GetTriangleCount() { return this->TriangleCount; }
	void SetFPSMAX( float maxFPS );
	void SetSunLightProperties(Vector3 direction, Vector3 lightColor, float intensity);
	void SetSceneAmbientLight(D3DXVECTOR3 amb) { this->sceneAmbientLight = amb; }
	D3DXVECTOR3 GetSceneAmbientLight() const { return this->sceneAmbientLight; }
	void ResizeEngine(float width, float height);
};

#endif