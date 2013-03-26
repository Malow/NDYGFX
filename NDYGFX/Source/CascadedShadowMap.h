#pragma once

#include "MaloW.h"
#include "MaloWFileDebug.h"
#include "GraphicsEngineParameters.h"
#include "Camera.h"

#define SHADOW_MAP_CASCADE_COUNT 3
#define SHADOW_SPLIT_LOG_FACTOR 0.9f
#define SHADOW_MAP_DEPTH_BIAS 0.01f

class CascadedShadowMap
{
private:
	GraphicsEngineParams params;
	int quality;
	float blendDistance;

	ID3D11Texture2D* shadowMap[SHADOW_MAP_CASCADE_COUNT];
	ID3D11DepthStencilView* shadowMapDepthView[SHADOW_MAP_CASCADE_COUNT];
	ID3D11ShaderResourceView* shadowMapSRView[SHADOW_MAP_CASCADE_COUNT];
	D3D11_VIEWPORT ShadowMapViewPort[SHADOW_MAP_CASCADE_COUNT];

	D3DXMATRIX viewProj[SHADOW_MAP_CASCADE_COUNT];

	float shadowMappingSplitDepths[SHADOW_MAP_CASCADE_COUNT + 1];
	D3DXPLANE** cascadePlanes; 

	float radii[SHADOW_MAP_CASCADE_COUNT];
	D3DXVECTOR3 previousMinValues[SHADOW_MAP_CASCADE_COUNT]; 

	void CalcShadowMappingSplitDepths();
	void CalcShadowMapMatrices(D3DXVECTOR3 sunLight, Camera* cam, int i, bool forceUpdate);

public:
	CascadedShadowMap();
	virtual ~CascadedShadowMap();

	void Init(ID3D11Device* g_Device, int quality);
	/*
		float nearPlaneDistanceCloserToSun - determines how much closer the near plane of the shadow map shall be to the sun to include potential occluders. This distance is added for each shadow map.
	*/
	void PreRender(D3DXVECTOR3& sunLight, Camera* cam);

	float GetBlendDistance() { return this->blendDistance; }
	unsigned int GetNrOfCascadeLevels() const { return SHADOW_MAP_CASCADE_COUNT; }
	D3D11_VIEWPORT& GetShadowMapViewPort(int i) { return this->ShadowMapViewPort[i]; }
	ID3D11ShaderResourceView* GetShadowMapSRV(int i) const { return this->shadowMapSRView[i]; }
	ID3D11DepthStencilView* GetShadowMapDSV(int i) const { return this->shadowMapDepthView[i]; }
	ID3D11Texture2D* GetShadowMap(int i) const { return this->shadowMap[i]; }
	D3DXMATRIX& GetViewProjMatrix(int i);
	float GetSplitDepth(int i) { return this->shadowMappingSplitDepths[i]; }
	D3DXPLANE GetCascadePlane(unsigned int cascadeIndex, unsigned int planeIndex) { return this->cascadePlanes[cascadeIndex][planeIndex]; } 
	D3DXPLANE* GetCascadePlanes(unsigned int cascadeIndex) { return this->cascadePlanes[cascadeIndex]; } 
	void CalcCascadePlanes(); 
	float SetBlendDistance(float blendDist) { this->blendDistance = blendDist; }

	void ResizeShadowmaps(ID3D11Device* g_Device, int qual);
};