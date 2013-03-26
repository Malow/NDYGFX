#ifndef DXMANAGER_H
#define DXMANAGER_H


//#define MALOWTESTPERF


#include "Decal.h"
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
#include "Billboard.h"
#include "BillboardCollection.h"
#include "SSAO.h"
#include "Text.h"
#include "SkyBox.h"
#include "FXAA.h"
#include "StaticMesh.h"
#include "AnimatedMesh.h"
#include "PhysicsEngine.h"
#include "WaterPlane.h"
#include "FBXMesh.h"
#include "DxManagerEvents.h"
#include "CascadedShadowMap.h"
#include "InstancingHelper.h"
#include "DxManagerHelperThread.h"

#ifdef MALOWTESTPERF
#include "MaloWPerformance.h"
#endif

#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
	#define INCLUDE_NDYGFX
#endif

class DxManager : public MaloW::Process
{
private:
	DxManagerHelperThread* helperThread;

	// Shaders
	Shader*	Shader_ForwardRendering;
	Shader* Shader_FBX;
	Shader* Shader_Skybox;
	Shader* Shader_Image;
	Shader* Shader_BillboardInstanced;
	Shader* Shader_Text;
	Shader* Shader_ShadowMap;
	Shader*	Shader_ShadowMapInstanced;
	Shader* Shader_ShadowMapAnimated;
	Shader* Shader_ShadowMapAnimatedInstanced;
	Shader* Shader_ShadowMapBillboardInstanced;
	Shader* Shader_DeferredGeometry; //Deprecated, replaced by Shader_DeferredGeometryInstanced
	Shader* Shader_DeferredGeometryInstanced;
	Shader* Shader_DeferredAnimatedGeometry; //Deprecated, replaced by Shader_DeferredAnimatedGeometryInstanced
	Shader* Shader_DeferredAnimatedGeometryInstanced;
	Shader* Shader_TerrainEditor;
	Shader* Shader_DeferredLightning;
	Shader* Shader_InvisibilityEffect;
	Shader* Shader_DeferredQuad;
	Shader* Shader_DeferredTexture;
	Shader* Shader_DeferredGeoTranslucent;
	Shader* Shader_DeferredPerPixelTranslucent;
	Shader* Shader_Fxaa;
	Shader* Shader_Decal;
	Shader* Shader_FogEnclosement;
	Shader* Shader_ShadowMapFBX;

#ifdef MALOWTESTPERF
	MaloWPerformance perf;
#endif

	// Standard stuff
	ID3D11DeviceContext* Dx_DeviceContext;
	ID3D11DepthStencilView* Dx_DepthStencilView;
	ID3D11Texture2D* Dx_DepthStencil;
	ID3D11RenderTargetView* Dx_RenderTargetView;
	IDXGISwapChain* Dx_SwapChain;
	ID3D11Device* Dx_Device;
	
	HWND hWnd;
	D3D11_VIEWPORT Dx_Viewport;
	
	//Own systems
	long framecount;
	GraphicsEngineParams params;
	Camera* camera;
	/*Used for rendering 2 circles on geometry at a given xz-coordinate in world space
	x = inner radius, y = outer radius, z&w = position in the xz-plane.*/
	D3DXVECTOR4 specialCircleParams; 
	MaloW::Array<Terrain*> terrains; //terrain = sector data
	MaloW::Array<StaticMesh*> objects;
	MaloW::Array<AnimatedMesh*> animations;
	MaloW::Array<WaterPlane*> waterplanes;
	MaloW::Array<FBXMesh*> FBXMeshes;
	MaloW::Array<Image*> images;
	MaloW::Array<Billboard*> billboards;
	MaloW::Array<BillboardCollection*> billboardCollections;
	MaloW::Array<Text*> texts;
	MaloW::Array<Decal*> decals;

	float LavaWavesOuterRadius;
	SkyBox* skybox;


	// Lightning
	MaloW::Array<Light*> lights;
	D3DXVECTOR3 sceneAmbientLight;
	SunLight sun;
	bool useSun;

	bool useShadow;
	CascadedShadowMap* csm;

	D3DXVECTOR3 fogColor;
	bool useEnclosingFog;
	Vector3 fogCenter;
	float fogRadius;
	float fogFadeFactor;

	// Deferred Rendering
	// Gbuffer:
	static const int NrOfRenderTargets = 4;
	ID3D11Texture2D* Dx_GbufferTextures[NrOfRenderTargets];
	ID3D11RenderTargetView* Dx_GbufferRTs[NrOfRenderTargets];
	ID3D11ShaderResourceView* Dx_GbufferSRVs[NrOfRenderTargets];

	ID3D11Texture2D* Dx_DeferredTexture;
	ID3D11RenderTargetView* Dx_DeferredQuadRT;
	ID3D11ShaderResourceView* Dx_DeferredSRV;

	SSAO* ssao;
	FXAA* fxaa;
	IBTHFbx* fbx;

	// Hardware instancing 
	InstancingHelper* instancingHelper;

	bool invisibleGeometry;

	// Timer
	float PCFreq;
	__int64 prevTimeStamp;

	float Timer;
	float LastCamUpdate;
	float LastFBXUpdate;
	float RendererSleep;

	bool DelayGettingCamera;
	int RenderedMeshes;
	int RenderedTerrains;
	int renderedTerrainShadows;
	int renderedMeshShadows;
	int renderedFBX;
	int NrOfDrawnVertices;
	int NrOfDrawCalls;
	int CurrentRenderedMeshes;
	int CurrentRenderedTerrains;
	int CurrentRenderedFBX;
	int CurrentRenderedNrOfVertices;
	int CurrentNrOfDrawCalls;

	// Needed for frustrum culling
	PhysicsEngine pe;
	D3DXPLANE FrustrumPlanes[6];

	//This Clears the scene(rendertargets & viewports) and function sets variables used by most shaders, such as camera position for instance.
	void PreRender();

	void RenderForward();
	void RenderDeferredGeoTerrains();
	void RenderDeferredGeoObjects();
	void RenderDeferredGeometryInstanced();
	void RenderDeferredPerPixel();
	void RenderInvisibilityEffect();
	void RenderParticles();
	void RenderShadowMap();
	void RenderCascadedShadowMap();
	void RenderCascadedShadowMapInstanced();
	void RenderImages();
	void RenderText();
	void RenderFBXMeshes();
	void RenderDeferredGeoTranslucent();
	void RenderDeferredPerPixelTranslucent();
	void RenderDecals();
	void RenderEnclosingFog();


	void RenderQuadDeferred();
	void RenderDeferredTexture();
	void RenderDeferredSkybox();
	void RenderAntiAliasing();

	void RenderBillboardsInstanced();

	//Atm, only just for debuggin. This just sets the number of meshes, shadows, vertices, drawcalls, etc that are done each frame.
	void PostRender();

	void HandleSetCameraEvent(SetCameraEvent* ev);
	void HandleTerrainEvent(TerrainEvent* me);
	void HandleStaticMeshEvent(StaticMeshEvent* me);
	void HandleAnimatedMeshEvent(AnimatedMeshEvent* me);
	void HandleLightEvent(LightEvent* le);
	void HandleImageEvent(ImageEvent* ie);
	void HandleBillboardEvent(BillboardEvent* ie);
	void HandleBillboardCollectionEvent(BillboardCollectionEvent* ie);
	void HandleTextEvent(TextEvent* te);
	void HandleWaterPlaneEvent(WaterPlaneEvent* ie);
	void HandleFBXEvent(FBXEvent* fe);
	void HandleReloadShaders(int shader);
	void HandleDecalEvent(DecalEvent* de);
	void HandleSkyBoxEvent(SkyBoxEvent* sbe);

	void CalculateCulling();

	void ResizeRenderer(ResizeEvent* ev);

	HRESULT Init();

	int TriangleCount;
	bool StartRender;
	void Render();

public:
	DxManager(HWND g_hWnd, GraphicsEngineParams params, Camera* cam);
	virtual ~DxManager();

	void StartRendering();
	virtual void Life();
	
	HRESULT Update(float deltaTime);

	void CreateSmokeEffect();
	void CreateTerrain(Terrain* terrain); 
	void CreateStaticMesh(StaticMesh* mesh);
	void CreateAnimatedMesh(AnimatedMesh* mesh);
	Object3D* createParticleObject(ParticleMesh* mesh);
	Light* CreateLight(D3DXVECTOR3 pos, bool UseShadowMap);
	void CreateImage(Image* image, string texture);
	void CreateBillboard(Billboard* billboard, string texture);
	void CreateBillboardCollection(BillboardCollection* billboardCollection, string texture);
	void CreateText(Text* text, string font);
	void CreateWaterPlane(WaterPlane* wp, string texture);
	void CreateSkyBox(string texture);
	void CreateFBXMesh(FBXMesh* mesh);
	void CreateDecal(Decal* decal, string texture);


	void SetFBXScene(IBTHFbx* fbx) { this->fbx = fbx; };


	void UseShadow(bool useShadow);
	void SetGrassFilePath(const char* flag);
	void RenderGrass(bool flag);
	bool GetRenderGrassFlag() const;
	void SetSpecialCircle(float innerRadius, float outerRadius, Vector2& targetPos);

	long GetFrameCount() const { return this->framecount; }

	void DeleteTerrain(Terrain* terrain);
	void DeleteStaticMesh(StaticMesh* mesh);
	void DeleteAnimatedMesh(AnimatedMesh* mesh);
	void DeleteLight(Light* light);
	void DeleteImage(Image* image);
	void DeleteBillboard(Billboard* billboard);
	void DeleteBillboardCollection(BillboardCollection* billboardCollection);
	void DeleteText(Text* text);
	void DeleteWaterPlane(WaterPlane* wp);
	void DeleteFBXMesh(FBXMesh* mesh);
	void DeleteDecal(Decal* decal);

	void SetCamera(Camera* cam);
	Camera* GetCamera() const;

	inline int GetTriangleCount() { return this->TriangleCount; }
	inline int GetMeshCount() { return this->objects.size() + this->animations.size(); }
	int GetRenderedMeshCount() const;
	inline int GetTerrainCount() const { return this->terrains.size(); }
	int GetRenderedTerrainCount() const; 
	int GetRenderedFBXCount() const;

	int GetRenderedMeshShadowCount() const;
	int GetRenderedTerrainShadowCount() const; 

	int GetNrOfDrawnVerticesCount() const;
	int GetNrOfDrawCallsCount() const;
	
	void SetMaxFPS(float maxFPS);
	float GetMaxFPS() const;
	void SetRendererSleep(float sleep);
	float GetRendererSleep() const;

	void ChangeShadowQuality(int newQual);
	void ReloadShaders(int shader);

	void SetEnclosingFog(Vector3 center, float radius, float fadeFactor);
	void SetFogColor(Vector3 color) { this->fogColor = D3DXVECTOR3(color.x, color.y, color.z); }

	void SetSunLightProperties(Vector3 direction, Vector3 lightColor, float intensity);
	void SetSunLightDisabled();
	void SetSceneAmbientLight(D3DXVECTOR3 amb) { this->sceneAmbientLight = amb; }
	SunLight GetSunLight() const { return this->sun; }
	D3DXVECTOR3 GetSceneAmbientLight() const { return this->sceneAmbientLight; }
	void ResizeEngine(unsigned int width, unsigned int height);
	ID3D11Device* GetDevice() { return this->Dx_Device; }
	ID3D11DeviceContext* GetContextDevice() { return this->Dx_DeviceContext; }
	void ResetPerfLogging();
	void PrintPerfLogging();
};

#endif