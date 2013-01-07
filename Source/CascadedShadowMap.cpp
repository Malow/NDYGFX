#include "CascadedShadowMap.h"

// EDIT: Alexivan, Macro Safety for lerp

#define lerp(t, a, b) ((a) + (t) * ((b) - (a)))

CascadedShadowMap::CascadedShadowMap()
{
	this->quality = 0;
	for(int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		this->shadowMap[i] = NULL;
		this->shadowMapDepthView[i] = NULL;
		this->shadowMapSRView[i] = NULL;
	}
}

CascadedShadowMap::~CascadedShadowMap()
{
	for(int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		if(this->shadowMapSRView[i])
			this->shadowMapSRView[i]->Release();
		if(this->shadowMap[i])
			this->shadowMap[i]->Release();
		if(this->shadowMapDepthView[i])
			this->shadowMapDepthView[i]->Release();
	}
}

void CascadedShadowMap::CalcShadowMapMatrices(D3DXVECTOR3 sunLight, Camera* cam, int i)
{
	D3DXMATRIX View;
	D3DXMatrixLookAtLH(&View, &(cam->GetPositionD3DX() - sunLight), &sunLight, &D3DXVECTOR3(0, 1, 0));

	float fNearPlane = this->shadowMappingSplitDepths[i];
	float fFarPlane = this->shadowMappingSplitDepths[i + 1];

	D3DXMATRIX Proj;
	float size = pow(5.0f, (i + 1.0f));
	D3DXMatrixOrthoOffCenterLH(&Proj, -size, size, -size, size,	0.001f, fFarPlane);
	this->viewProj[i] = View * Proj;

	/*
	D3DXMATRIX viewportToTex = D3DXMATRIX(
		0.5f,  0.f,  0.f, 0.f,
		0.f,  -0.5f, 0.f, 0.f,
		0.f,   0.f,  1.f, 0.f,
		0.5f,  0.5f, SHADOW_MAP_DEPTH_BIAS, 0.f);
	outShadowMapTexXform = outViewProj * viewportToTex;
	*/
}

void CascadedShadowMap::CalcShadowMappingSplitDepths()
{
	float camNear = this->params.NearClip;
	float camFar  = this->params.FarClip;

	this->shadowMappingSplitDepths[0] = camNear;
	this->shadowMappingSplitDepths[1] = camFar * 0.1f;
	this->shadowMappingSplitDepths[2] = camFar * 0.4f;
	this->shadowMappingSplitDepths[SHADOW_MAP_CASCADE_COUNT] = camFar;
	/*
	float i_f = 1.0f;
	float cascadeCount = (float)SHADOW_MAP_CASCADE_COUNT;

	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		this->shadowMappingSplitDepths[i] = lerp(camNear + (i_f/cascadeCount)*(camFar - camNear), 
			camNear * powf(camFar / camNear, i_f/cascadeCount),
			SHADOW_SPLIT_LOG_FACTOR);

		i_f += 1.f;
	}
	this->shadowMappingSplitDepths[SHADOW_MAP_CASCADE_COUNT] = camFar;
	*/
}

void CascadedShadowMap::Init(ID3D11Device* g_Device, int quality)
{
	this->quality = quality;
	int width = (int)(256 * pow(2.0f, quality/2));
	int height = (int)(256 * pow(2.0f, quality/2));

	for(int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		// Shadow Maps
		//create the depth stencil
		D3D11_TEXTURE2D_DESC shadowMapDescDepth;
		ZeroMemory(&shadowMapDescDepth, sizeof(shadowMapDescDepth));
		shadowMapDescDepth.Width = width;
		shadowMapDescDepth.Height = height;
		shadowMapDescDepth.MipLevels = 1;
		shadowMapDescDepth.ArraySize = 1;
		shadowMapDescDepth.Format = DXGI_FORMAT_R32_TYPELESS;
		shadowMapDescDepth.SampleDesc.Count = 1;
		shadowMapDescDepth.SampleDesc.Quality = 0;
		shadowMapDescDepth.Usage = D3D11_USAGE_DEFAULT;
		shadowMapDescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		shadowMapDescDepth.CPUAccessFlags = 0;
		shadowMapDescDepth.MiscFlags = 0;

		if(FAILED(g_Device->CreateTexture2D(&shadowMapDescDepth, NULL, &shadowMap[i])))
			MaloW::Debug("Cascaded Smaps failed.");


		D3D11_DEPTH_STENCIL_VIEW_DESC descDSVShadowMap;
		ZeroMemory(&descDSVShadowMap, sizeof(descDSVShadowMap));
		descDSVShadowMap.Format = DXGI_FORMAT_D32_FLOAT;
		descDSVShadowMap.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSVShadowMap.Texture2D.MipSlice = 0;

		if(FAILED(g_Device->CreateDepthStencilView(shadowMap[i], &descDSVShadowMap, &shadowMapDepthView[i])))
			MaloW::Debug("Cascaded Smaps failed.");


		// Create the shader-resource view from the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		ZeroMemory(&srDesc, sizeof(srDesc));
		srDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = 1;

		if(FAILED(g_Device->CreateShaderResourceView(shadowMap[i], &srDesc, &shadowMapSRView[i])))
			MaloW::Debug("Cascaded Smaps failed.");


		ShadowMapViewPort[i].Width = (float)width;
		ShadowMapViewPort[i].Height = (float)height;
		ShadowMapViewPort[i].MinDepth = 0.0f;
		ShadowMapViewPort[i].MaxDepth = 1.0f;
		ShadowMapViewPort[i].TopLeftX = 0;
		ShadowMapViewPort[i].TopLeftY = 0;	
	}

	CalcShadowMappingSplitDepths();
}

D3DXMATRIX CascadedShadowMap::GetViewProjMatrix(int i)
{
	return this->viewProj[i];
}

void CascadedShadowMap::PreRender(D3DXVECTOR3 sunLight, Camera* cam)
{
	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		CalcShadowMapMatrices(sunLight, cam, i);
	}
}