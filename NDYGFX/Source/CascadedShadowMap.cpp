#include "CascadedShadowMap.h"

// EDIT: Alexivan, Macro Safety for lerp

#define lerp(t, a, b) ((a) + (t) * ((b) - (a)))

CascadedShadowMap::CascadedShadowMap()
{
	this->quality = 0;
	this->blendDistance = 0.0f; 
	this->cascadePlanes = new D3DXPLANE*[SHADOW_MAP_CASCADE_COUNT];
	for(int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		this->shadowMap[i] = NULL;
		this->shadowMapDepthView[i] = NULL;
		this->shadowMapSRView[i] = NULL;
		this->cascadePlanes[i] = new D3DXPLANE[6];

		previousMinValues[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	//Todo: replace with algorithm using FOV and cascade levels(distance)(so that all cascades update at the same time).
	radii[0] = 4.0f; //0.1("far" clip) * 40
	radii[1] = 16.0f;  //0.4("far" clip) * 40
	radii[2] = 40.0f; //1.0("far" clip) * 40
	
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
		if(this->cascadePlanes[i]) delete[] this->cascadePlanes[i]; this->cascadePlanes[i] = NULL;
	}
	if(this->cascadePlanes) delete[] this->cascadePlanes; this->cascadePlanes = NULL;
}

void CascadedShadowMap::CalcShadowMapMatrices(D3DXVECTOR3 sunLight, Camera* cam, int i, bool forceUpdate)
{
	//Ändra getforward/right/up till getOldforward/right/up?
	//calculate points (in world space) for the frustum slice
	D3DXVECTOR4 camPos = D3DXVECTOR4(cam->GetOldPos(), 1.0f);
	D3DXVECTOR4 camForward = D3DXVECTOR4(cam->GetForwardD3DX(), 1.0f);
	D3DXVECTOR4 camRight = D3DXVECTOR4(cam->GetRightVectorD3DX(), 1.0f);
	D3DXVECTOR4 camUp = D3DXVECTOR4(cam->GetUpVectorD3DX(), 1.0f);
	float aspectRatio = (float)this->params.WindowWidth / (float)this->params.WindowHeight;
	float tmp = tan((this->params.FOV * 0.0174532925f) / 3.14159265359f) * 2;

	//Near plane	
	float halfNearHeight = tmp * this->shadowMappingSplitDepths[i];
	float halfNearWidth = halfNearHeight * aspectRatio;

	//Near points
	D3DXVECTOR4	nearCenter = camPos + camForward * this->shadowMappingSplitDepths[i]; //near plane of slice
	D3DXVECTOR4	nearTopLeft = nearCenter + (camUp * halfNearHeight) - (camRight * halfNearWidth);
	D3DXVECTOR4	nearTopRight = nearCenter + (camUp * halfNearHeight) + (camRight * halfNearWidth);
	D3DXVECTOR4	nearBottomLeft = nearCenter - (camUp * halfNearHeight) - (camRight * halfNearWidth);
	D3DXVECTOR4	nearBottomRight = nearCenter - (camUp * halfNearHeight) + (camRight * halfNearWidth);

	//Far plane
	float halfFarHeight = tmp * this->shadowMappingSplitDepths[i + 1];
	float halfFarWidth = halfFarHeight * aspectRatio;
	//Far points
	D3DXVECTOR4 farCenter = camPos + camForward * this->shadowMappingSplitDepths[i + 1]; //far plane of slice
	D3DXVECTOR4 farTopLeft = farCenter + (camUp * halfFarHeight) - (camRight * halfFarWidth);
	D3DXVECTOR4	farTopRight = farCenter + (camUp * halfFarHeight) + (camRight * halfFarWidth);
	D3DXVECTOR4	farBottomLeft = farCenter - (camUp * halfFarHeight) - (camRight * halfFarWidth);
	D3DXVECTOR4	farBottomRight = farCenter - (camUp * halfFarHeight) + (camRight * halfFarWidth);

	//Put them in an array
	D3DXVECTOR4 frustumPoints[8] = {nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight,
									farTopLeft, farTopRight, farBottomLeft, farBottomRight };
	
	// Normalize Sunlight
	D3DXVec3Normalize(&sunLight, &sunLight);

	//Calculate the light's view matrix.
	D3DXVECTOR3 lightPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 
	D3DXVECTOR3 lightUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMATRIX lightViewMatrix;
	D3DXMatrixIdentity(&lightViewMatrix);
	D3DXMatrixLookAtLH(&lightViewMatrix, &lightPos, &sunLight, &lightUp);  //sunlight = the direction the sun is "looking at".
	
	//Transform the points from world space to light’s homogeneous space.
	for(int index = 0; index < 8; index++)
	{
		//reset W
		frustumPoints[index].w = 1.0f;
		D3DXVec4Transform(&frustumPoints[index], &frustumPoints[index], &lightViewMatrix);
	}

	//Calculate the min(near plane) and max(far plane) values for the orthographic projection.
	float infinity = std::numeric_limits<float>::infinity();
	D3DXVECTOR3 minValue = D3DXVECTOR3(infinity, infinity, infinity);
	D3DXVECTOR3 maxValue = D3DXVECTOR3(-infinity, -infinity, -infinity);
	for(int index = 0; index < 8; index++)
	{
		D3DXVECTOR3 vec3 = D3DXVECTOR3(frustumPoints[index].x, frustumPoints[index].y, frustumPoints[index].z);
		
		//Find the min & max points.
		D3DXVec3Minimize(&minValue, &minValue, &vec3);
		D3DXVec3Maximize(&maxValue, &maxValue, &vec3);
	}
	
	
	//Calculate distance between previous min point.
	D3DXVECTOR3 tmpVec = minValue - this->previousMinValues[i];
	float len = D3DXVec3Length(&tmpVec);
	if(len > radii[i])
	{
		this->previousMinValues[i] = minValue;
	}
	else
	{
		//If the point is still within the radius and is not forced to update by another cascade,
		//cascade does not need to be re-calculated.
		
		if(!forceUpdate)
		{
			return;
		}
	}
	

	D3DXVECTOR3 tmpNearPlanePoint = minValue;
	D3DXVECTOR3 tmpFarPlanePoint = maxValue;
	//Set the near plane to be closer to the light to include more potential occluders.
	if(sunLight.z < 0.0f)
	{
		sunLight *= -1.0f;
	}
	//Todo: set same near plane for all cascades
	tmpNearPlanePoint -= sunLight * (float)(this->params.ShadowFit * (i + 1)); 
	tmpNearPlanePoint += sunLight * this->radii[i]; //Move near plane closer to sun(expand cascade)
	tmpFarPlanePoint -= sunLight * this->radii[i]; //Move far plane further from sun(expand cascade)
	float nearPlane = tmpNearPlanePoint.z; 
	float farPlane = maxValue.z;

	// Create the orthographic projection for this cascade/frustum slice.
	// Add the blend distance to make the projections overlap each other for blending between the shadow maps.
	D3DXMATRIX lightProjMatrix;
	D3DXMatrixIdentity(&lightProjMatrix);
	//Todo: Blend distance makes the projections overlap
	//This overlap is used to smooth/blur the edges where the cascades meet.
	D3DXMatrixOrthoOffCenterLH( &lightProjMatrix, 
		minValue.x - this->blendDistance - this->radii[i], //Offset with radius(expand cascade)*
		maxValue.x + this->blendDistance + this->radii[i], //Offset with radius(expand cascade)*
		minValue.y - this->blendDistance - this->radii[i], //Offset with radius(expand cascade)*
		maxValue.y + this->blendDistance + this->radii[i], //Offset with radius(expand cascade)*
		nearPlane, farPlane);
	//*Performance wise, this is not ideal as a bigger projection means more objects sent to the GFXcard and rendered.
	
	this->viewProj[i] = lightViewMatrix * lightProjMatrix;

	//Force all other cascades (below) to update (higher cascades update more often than lower)
	if(i > 0)
	{
		this->CalcShadowMapMatrices(sunLight, cam, i - 1, true);
	}
}

void CascadedShadowMap::CalcShadowMappingSplitDepths()
{
	this->shadowMappingSplitDepths[0] = this->params.NearClip;

	/*this->shadowMappingSplitDepths[1] = this->params.FarClip * 0.025f;
	this->shadowMappingSplitDepths[2] = this->params.FarClip * 0.1f;
	this->shadowMappingSplitDepths[3] = this->params.FarClip * 0.4f;
	this->shadowMappingSplitDepths[SHADOW_MAP_CASCADE_COUNT] = this->params.FarClip;
	*/
	this->shadowMappingSplitDepths[1] = this->params.FarClip * 0.1f;
	this->shadowMappingSplitDepths[2] = this->params.FarClip * 0.4f;
	this->shadowMappingSplitDepths[SHADOW_MAP_CASCADE_COUNT] = this->params.FarClip;

	//Todo: algorithm depending on SHADOW_MAP_CASCADE_COUNT
}
void CascadedShadowMap::CalcCascadePlanes()
{
	// Calculate near plane of frustum.
	for(int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		this->cascadePlanes[i][0].a = this->viewProj[i]._14 + this->viewProj[i]._13;
		this->cascadePlanes[i][0].b = this->viewProj[i]._24 + this->viewProj[i]._23;
		this->cascadePlanes[i][0].c = this->viewProj[i]._34 + this->viewProj[i]._33;
		this->cascadePlanes[i][0].d = this->viewProj[i]._44 + this->viewProj[i]._43;
		D3DXPlaneNormalize(&this->cascadePlanes[i][0], &this->cascadePlanes[i][0]);

		// Calculate far plane of frustum.
		this->cascadePlanes[i][1].a = this->viewProj[i]._14 - this->viewProj[i]._13; 
		this->cascadePlanes[i][1].b = this->viewProj[i]._24 - this->viewProj[i]._23;
		this->cascadePlanes[i][1].c = this->viewProj[i]._34 - this->viewProj[i]._33;
		this->cascadePlanes[i][1].d = this->viewProj[i]._44 - this->viewProj[i]._43;
		D3DXPlaneNormalize(&this->cascadePlanes[i][1], &this->cascadePlanes[i][1]);

		// Calculate left plane of frustum.
		this->cascadePlanes[i][2].a = this->viewProj[i]._14 + this->viewProj[i]._11; 
		this->cascadePlanes[i][2].b = this->viewProj[i]._24 + this->viewProj[i]._21;
		this->cascadePlanes[i][2].c = this->viewProj[i]._34 + this->viewProj[i]._31;
		this->cascadePlanes[i][2].d = this->viewProj[i]._44 + this->viewProj[i]._41;
		D3DXPlaneNormalize(&this->cascadePlanes[i][2], &this->cascadePlanes[i][2]);

		// Calculate right plane of frustum.
		this->cascadePlanes[i][3].a = this->viewProj[i]._14 - this->viewProj[i]._11; 
		this->cascadePlanes[i][3].b = this->viewProj[i]._24 - this->viewProj[i]._21;
		this->cascadePlanes[i][3].c = this->viewProj[i]._34 - this->viewProj[i]._31;
		this->cascadePlanes[i][3].d = this->viewProj[i]._44 - this->viewProj[i]._41;
		D3DXPlaneNormalize(&this->cascadePlanes[i][3], &this->cascadePlanes[i][3]);

		// Calculate top plane of frustum.
		this->cascadePlanes[i][4].a = this->viewProj[i]._14 - this->viewProj[i]._12; 
		this->cascadePlanes[i][4].b = this->viewProj[i]._24 - this->viewProj[i]._22;
		this->cascadePlanes[i][4].c = this->viewProj[i]._34 - this->viewProj[i]._32;
		this->cascadePlanes[i][4].d = this->viewProj[i]._44 - this->viewProj[i]._42;
		D3DXPlaneNormalize(&this->cascadePlanes[i][4], &this->cascadePlanes[i][4]);

		// Calculate bottom plane of frustum.
		this->cascadePlanes[i][5].a = this->viewProj[i]._14 + this->viewProj[i]._12;
		this->cascadePlanes[i][5].b = this->viewProj[i]._24 + this->viewProj[i]._22;
		this->cascadePlanes[i][5].c = this->viewProj[i]._34 + this->viewProj[i]._32;
		this->cascadePlanes[i][5].d = this->viewProj[i]._44 + this->viewProj[i]._42;
		D3DXPlaneNormalize(&this->cascadePlanes[i][5], &this->cascadePlanes[i][5]);
	}
}

void CascadedShadowMap::Init(ID3D11Device* g_Device, int quality)
{
	this->quality = quality;
	int width = (int)(256.0f * powf(2.0f, (float)quality * 0.5f));
	int height = (int)(256.0f * powf(2.0f, (float)quality * 0.5f));

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

D3DXMATRIX& CascadedShadowMap::GetViewProjMatrix(int i)
{
	return this->viewProj[i];
}

void CascadedShadowMap::PreRender(D3DXVECTOR3& sunLight, Camera* cam)
{
	this->CalcShadowMappingSplitDepths();
	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		CalcShadowMapMatrices(sunLight, cam, i, false);
	}
}

void CascadedShadowMap::ResizeShadowmaps(ID3D11Device* g_Device, int qual )
{
	this->quality = qual;
	int width = (int)(256.0f * powf(2.0f, (float)quality * 0.5f));
	int height = (int)(256.0f * powf(2.0f, (float)quality * 0.5f));


	for(int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		if(this->shadowMapSRView[i])
			this->shadowMapSRView[i]->Release();
		if(this->shadowMap[i])
			this->shadowMap[i]->Release();
		if(this->shadowMapDepthView[i])
			this->shadowMapDepthView[i]->Release();
	}


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
