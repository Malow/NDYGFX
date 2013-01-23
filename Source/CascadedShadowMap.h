#pragma once

#include "MaloW.h"
#include "MaloWFileDebug.h"
#include "GraphicsEngineParameters.h"
#include "Camera.h"

#define SHADOW_MAP_CASCADE_COUNT 4
#define SHADOW_SPLIT_LOG_FACTOR 0.9f
#define SHADOW_MAP_DEPTH_BIAS 0.01f

class CascadedShadowMap
{
private:
	GraphicsEngineParams params;
	int quality;

	ID3D11Texture2D* shadowMap[SHADOW_MAP_CASCADE_COUNT];
	ID3D11DepthStencilView* shadowMapDepthView[SHADOW_MAP_CASCADE_COUNT];
	ID3D11ShaderResourceView* shadowMapSRView[SHADOW_MAP_CASCADE_COUNT];
	D3D11_VIEWPORT ShadowMapViewPort[SHADOW_MAP_CASCADE_COUNT];

	D3DXMATRIX viewProj[SHADOW_MAP_CASCADE_COUNT];

	float shadowMappingSplitDepths[SHADOW_MAP_CASCADE_COUNT + 1];

	void CalcShadowMappingSplitDepths();
	void CalcShadowMapMatrices(D3DXVECTOR3 sunLight, Camera* cam, int i);

public:
	CascadedShadowMap();
	virtual ~CascadedShadowMap();

	void Init(ID3D11Device* g_Device, int quality);
	void PreRender(D3DXVECTOR3 sunLight, Camera* cam);

	int GetNrOfCascadeLevels() { return SHADOW_MAP_CASCADE_COUNT; }
	D3D11_VIEWPORT GetShadowMapViewPort(int i) const { return this->ShadowMapViewPort[i]; }
	ID3D11ShaderResourceView* GetShadowMapSRV(int i) const { return this->shadowMapSRView[i]; }
	ID3D11DepthStencilView* GetShadowMapDSV(int i) const { return this->shadowMapDepthView[i]; }
	ID3D11Texture2D* GetShadowMap(int i) const { return this->shadowMap[i]; }
	D3DXMATRIX GetViewProjMatrix(int i);
	float GetSplitDepth(int i) { return this->shadowMappingSplitDepths[i]; }
};