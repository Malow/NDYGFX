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

void CascadedShadowMap::CalcShadowMapMatrices(D3DXVECTOR3 sunLight, Camera* cam, int i, float nearPlaneDistanceCloserToSun)
{
	//calculate points (in world space) for the frustum slice
	D3DXVECTOR4 camPos = D3DXVECTOR4(cam->GetPositionD3DX(), 1.0f);
	D3DXVECTOR4 camForward = D3DXVECTOR4(cam->GetForwardD3DX(), 1.0f);
	D3DXVECTOR4 camRight = D3DXVECTOR4(cam->GetRightVectorD3DX(), 1.0f);
	D3DXVECTOR4 camUp = D3DXVECTOR4(cam->GetUpVectorD3DX(), 1.0f);
	float ww = (float)this->params.windowWidth;
	float hh = (float)this->params.windowHeight;
	float aspectRatio = ww / hh;
	float tmp = tan(this->params.FOV) * 2;

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
	
	
	


	//Calculate the light's view matrix.
	D3DXVECTOR3 lightPos = D3DXVECTOR3(-1000, 1000, -1000); //**tillman**
	D3DXVECTOR3 lightLookAt = lightPos + sunLight; //sunlight = the direction the sun is "looking at".
	D3DXVECTOR3 lightUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMATRIX lightViewMatrix;
	D3DXMatrixIdentity(&lightViewMatrix);
	D3DXMatrixLookAtLH(&lightViewMatrix, &lightPos, &lightLookAt, &lightUp); 
	
	//Transform the points from world space to light’s homogeneous space.
	for(int index = 0; index < 8; index++)
	{
		//reset W
		frustumPoints[index].w = 1.0f;
		D3DXVec4Transform(&frustumPoints[index], &frustumPoints[index], &lightViewMatrix);
	}
	

	//Calculate the min and max values for the orthographic projection.*A*
	//**tillman, find the near and far plane *A*
	float infinity = std::numeric_limits<float>::infinity();
	D3DXVECTOR3 minValue = D3DXVECTOR3(infinity, infinity, infinity);
	D3DXVECTOR3 maxValue = D3DXVECTOR3(-infinity, -infinity, -infinity);
	D3DXVECTOR3 vLightCameraOrthographicMin = D3DXVECTOR3(infinity, infinity, infinity); //**tillman pot opt, samma som minValue
	D3DXVECTOR3 vLightCameraOrthographicMax = D3DXVECTOR3(-infinity, -infinity, -infinity);//**tillman pot opt, samma som maxValue
	for(int index = 0; index < 8; index++)
	{
		D3DXVECTOR3 vec3 = D3DXVECTOR3(frustumPoints[index].x, frustumPoints[index].y, frustumPoints[index].z);

		D3DXVec3Minimize(&minValue, &minValue, &vec3);
		D3DXVec3Maximize(&maxValue, &maxValue, &vec3);

		// Find the closest point.
		D3DXVec3Minimize (&vLightCameraOrthographicMin, &vec3, &vLightCameraOrthographicMin );
		D3DXVec3Maximize (&vLightCameraOrthographicMax, &vec3, &vLightCameraOrthographicMax );
	}
	D3DXVECTOR3 tmpNearPlanePoint = minValue;
	//set the near plane to be closer to the light to include more potential occluders.
	tmpNearPlanePoint -= sunLight * nearPlaneDistanceCloserToSun * (i + 1); 
	float nearPlane = tmpNearPlanePoint.z; 
	float farPlane = maxValue.z;

	// Create the orthographic projection for this cascade/frustum slice.
	D3DXMATRIX lightProjMatrix;
	D3DXMatrixIdentity(&lightProjMatrix);
	D3DXMatrixOrthoOffCenterLH( &lightProjMatrix, 
		vLightCameraOrthographicMin.x, 
		vLightCameraOrthographicMax.x, 
		vLightCameraOrthographicMin.y, 
		vLightCameraOrthographicMax.y, 
		nearPlane, farPlane);

	this->viewProj[i] = lightViewMatrix * lightProjMatrix;
}

void CascadedShadowMap::CalcShadowMappingSplitDepths()
{
	float camNear = this->params.NearClip;
	float camFar  = this->params.FarClip;

	this->shadowMappingSplitDepths[0] = camNear;
	/*float scale = ;
	for(int i = 1; i < SHADOW_MAP_CASCADE_COUNT; i++) //TILLMAN TODO
	{
		this->shadowMappingSplitDepths[i] = camFar * scale * 0.001f;
	}*/
	this->shadowMappingSplitDepths[1] = camFar * 0.025f;
	this->shadowMappingSplitDepths[2] = camFar * 0.1f;
	this->shadowMappingSplitDepths[3] = camFar * 0.4f;
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
	int width = (int)(256.0f * pow(2.0f, quality / 2.0f));
	int height = (int)(256.0f * pow(2.0f, quality / 2.0f));

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

void CascadedShadowMap::PreRender(D3DXVECTOR3 sunLight, Camera* cam, float nearPlaneDistanceCloserToSun)
{
	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		CalcShadowMapMatrices(sunLight, cam, i, nearPlaneDistanceCloserToSun);
	}
}