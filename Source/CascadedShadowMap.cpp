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
	//calculate points (in world space) for the frustum slice
	D3DXVECTOR4 camPos = D3DXVECTOR4(cam->GetPositionD3DX(), 1.0f);
	D3DXVECTOR4 camForward = D3DXVECTOR4(cam->GetForwardD3DX(), 1.0f);
	D3DXVECTOR4 camRight = D3DXVECTOR4(cam->GetRightVectorD3DX(), 1.0f);
	D3DXVECTOR4 camUp = D3DXVECTOR4(cam->GetUpVectorD3DX(), 1.0f);
	float ww = this->params.windowWidth;
	float hh = this->params.windowHeight;
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
	
	
	//**tillman, code above should work (liknande kod är testad & fungerar)




	//Calculate the light's view matrix.
	D3DXVECTOR3 lightPos = D3DXVECTOR3(-1000, 1000, -1000); //D3DXVECTOR3(0.0f, 99999.9f, 0.0f); //**tillman** D3DXVECTOR3(-50, 50, -50);
	D3DXVECTOR3 lightLookAt = lightPos + sunLight;
	D3DXVECTOR3 lightUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMATRIX lightViewMatrix;
	D3DXMatrixIdentity(&lightViewMatrix);
	D3DXMatrixLookAtLH(&lightViewMatrix, &lightPos, &lightLookAt, &lightUp); 
	
	//Transform the points from world space to light’s homogeneous space.
	for(int i = 0; i < 8; i++)
	{
		//reset W
		frustumPoints[i].w = 1.0f;
		D3DXVec4Transform(&frustumPoints[i], &frustumPoints[i], &lightViewMatrix);
	}
	

	//Calculate the min and max values for the orthographic projection.*A*
	//**tillman, find the near and far plane *A*
	float infinity = std::numeric_limits<float>::infinity();
	D3DXVECTOR3 minValue = D3DXVECTOR3(infinity, infinity, infinity);
	D3DXVECTOR3 maxValue = D3DXVECTOR3(-infinity, -infinity, -infinity);
	D3DXVECTOR3 vLightCameraOrthographicMin = D3DXVECTOR3(infinity, infinity, infinity); //**tillman, samma som minValue
	D3DXVECTOR3 vLightCameraOrthographicMax = D3DXVECTOR3(-infinity, -infinity, -infinity);//**tillman, samma som maxValue
	for(int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 vec3 = D3DXVECTOR3(frustumPoints[i].x, frustumPoints[i].y, frustumPoints[i].z);

		D3DXVec3Minimize(&minValue, &minValue, &vec3);
		D3DXVec3Maximize(&maxValue, &maxValue, &vec3);

		// Find the closest point.
		D3DXVec3Minimize (&vLightCameraOrthographicMin, &vec3, &vLightCameraOrthographicMin );
		D3DXVec3Maximize (&vLightCameraOrthographicMax, &vec3, &vLightCameraOrthographicMax );
	}
	float nearPlane = minValue.z;
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

	


	/* //**Tillman: old code**
	//Transform points into light’s homogeneous space.
	D3DXMATRIX lightViewMatrix; //M
	D3DXMatrixLookAtLH(&lightViewMatrix, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &sunLight, &D3DXVECTOR3(0, 1, 0)); //**
	D3DXMATRIX lightProjMatrix; //P
	D3DXMatrixIdentity(&lightProjMatrix);
	//Far
	D3DXVECTOR4 farTopLeftH = D3DXVECTOR4(farTopLeft.x, farTopLeft.y, farTopLeft.z, 1.0f);
	D3DXVECTOR4 farTopRightH = D3DXVECTOR4(farTopRight.x, farTopRight.y, farTopRight.z, 1.0f);
	D3DXVECTOR4	farBottomLeftH = D3DXVECTOR4(farBottomLeft.x, farBottomLeft.y, farBottomLeft.z, 1.0f);
	D3DXVECTOR4	farBottomRightH = D3DXVECTOR4(farBottomRight.x, farBottomRight.y, farBottomRight.z, 1.0f);
	//Near
	D3DXVECTOR4	nearTopLeftH = D3DXVECTOR4(nearTopLeft.x, nearTopLeft.y, nearTopLeft.z, 1.0f);
	D3DXVECTOR4	nearTopRightH = D3DXVECTOR4(nearTopRight.x, nearTopRight.y, nearTopRight.z, 1.0f);
	D3DXVECTOR4	nearBottomLeftH = D3DXVECTOR4(nearBottomLeft.x, nearBottomLeft.y, nearBottomLeft.z, 1.0f);
	D3DXVECTOR4	nearBottomRightH = D3DXVECTOR4(nearBottomRight.x, nearBottomRight.y, nearBottomRight.z, 1.0f);
	D3DXMATRIX lightViewProjMatrix = lightProjMatrix * lightViewMatrix;
	//Far
	D3DXVec4Transform(&farTopLeftH, &farTopLeftH, &lightViewProjMatrix);
	D3DXVec4Transform(&farTopRightH, &farTopRightH, &lightViewProjMatrix);
	D3DXVec4Transform(&farBottomLeftH, &farBottomLeftH, &lightViewProjMatrix);
	D3DXVec4Transform(&farBottomRightH, &farBottomRightH, &lightViewProjMatrix);
	//Near
	D3DXVec4Transform(&nearTopLeftH, &nearTopLeftH, &lightViewProjMatrix);
	D3DXVec4Transform(&nearTopRightH, &nearTopRightH, &lightViewProjMatrix);
	D3DXVec4Transform(&nearBottomLeftH, &nearBottomLeftH, &lightViewProjMatrix);
	D3DXVec4Transform(&nearBottomRightH, &nearBottomRightH, &lightViewProjMatrix);

	//Calculate crop matrix
	//Find min & max values in each dimension
	float minX = min(farTopLeftH.x, min(farTopRightH.x, min(farBottomLeftH.x, min(farBottomRightH.x, 
		min(nearTopLeftH.x, min(nearTopRightH.x, min(nearBottomLeftH.x, nearBottomRightH.x)))))));
	float maxX = max(farTopLeftH.x, max(farTopRightH.x, max(farBottomLeftH.x, max(farBottomRightH.x, 
		max(nearTopLeftH.x, max(nearTopRightH.x, max(nearBottomLeftH.x, nearBottomRightH.x)))))));
	float minY = min(farTopLeftH.y, min(farTopRightH.y, min(farBottomLeftH.y, min(farBottomRightH.y, 
		min(nearTopLeftH.y, min(nearTopRightH.y, min(nearBottomLeftH.y, nearBottomRightH.y)))))));
	float maxY = max(farTopLeftH.y, max(farTopRightH.y, max(farBottomLeftH.y, max(farBottomRightH.y, 
		max(nearTopLeftH.y, max(nearTopRightH.y, max(nearBottomLeftH.y, nearBottomRightH.y)))))));
	float minZ = min(farTopLeftH.z, min(farTopRightH.z, min(farBottomLeftH.z, min(farBottomRightH.z, 
		min(nearTopLeftH.z, min(nearTopRightH.z, min(nearBottomLeftH.z, nearBottomRightH.z)))))));
	float maxZ = max(farTopLeftH.z, max(farTopRightH.z, max(farBottomLeftH.z, max(farBottomRightH.z, 
		max(nearTopLeftH.z, max(nearTopRightH.z, max(nearBottomLeftH.z, nearBottomRightH.z)))))));
	
	//Determine scale
	float scaleX = 2 / maxX - minX;
	float scaleY = 2 / maxY - minY;
	//Determine offset
	float offsetX = -0.5f * (maxX + minX) * scaleX;
	float offsetY = -0.5f * (maxY + minY) * scaleY;

	//Create cropmatrix and projection matrix
	D3DXMATRIX cropMatrix, lightProjMatrix2; //lightProjMatrix2 (Pz)
	D3DXMatrixIdentity(&cropMatrix);
	cropMatrix._11 = scaleX;
	cropMatrix._22 = scaleY;
	cropMatrix._41 = offsetX;
	cropMatrix._42 = offsetY;
	//D3DXMatrixOrthoLH(&lightProjMatrix2, width, height, minZ, maxZ); //**width? height?

	//Lastly, modify the projection matrix (P=CPz)
	lightProjMatrix = cropMatrix * lightProjMatrix2;

	D3DXMATRIX View;
	D3DXMatrixLookAtLH(&View, &(cam->GetPositionD3DX() - sunLight), &sunLight, &D3DXVECTOR3(0, 1, 0));

	float fNearPlane = this->shadowMappingSplitDepths[i];
	float fFarPlane = this->shadowMappingSplitDepths[i + 1];

	D3DXMATRIX Proj;
	float size = pow(5.0f, (i + 1.0f));
	D3DXMatrixOrthoLH(&Proj, size, size, fNearPlane, fFarPlane);
	//D3DXMatrixOrthoOffCenterLH(&Proj, -size, size, -size, size,	0.001f, fFarPlane);
	this->viewProj[i] = View * Proj;
	*/
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