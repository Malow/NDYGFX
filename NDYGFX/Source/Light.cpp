#include "Light.h"
#include "Vector.h"

Light::Light()
{
	this->pos = D3DXVECTOR3(0, 0, 0);
	this->type = POINT_LIGHT;
	this->UseShadowMap = false;
	this->intensity = 10.0f;

	this->SetLookAt(Vector3(this->pos.x, this->pos.y, this->pos.z) + Vector3(0,-1,0));

	this->shadowMap = NULL;
	this->shadowMapDepthView = NULL;
	this->shadowMapSRView = NULL;
}

Light::Light(D3DXVECTOR3 pos, LightType type)
{
	this->pos = pos;
	this->type = type;
	this->UseShadowMap = false;
	this->intensity = 10.0f;

	this->SetLookAt(Vector3(this->pos.x, this->pos.y, this->pos.z) + Vector3(0,-1,0));

	this->shadowMap = NULL;
	this->shadowMapDepthView = NULL;
	this->shadowMapSRView = NULL;
	
}

Light::~Light() 
{ 
	if(this->shadowMapSRView)
		this->shadowMapSRView->Release();
	if(this->shadowMap)
		this->shadowMap->Release();
	if(this->shadowMapDepthView)
		this->shadowMapDepthView->Release();
}

HRESULT Light::InitShadowMap(ID3D11Device* g_Device, int quality)
{
	this->UseShadowMap = true;
	// Shadow Maps
	//create the depth stencil

	int width = (int)(256 * powf(2.0f, (float)quality * 0.5f));
	int height = (int)(256 * powf(2.0f, (float)quality * 0.5f));

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
	
	if(FAILED(g_Device->CreateTexture2D(&shadowMapDescDepth, NULL, &shadowMap)))
		return E_FAIL;


	D3D11_DEPTH_STENCIL_VIEW_DESC descDSVShadowMap;
	ZeroMemory(&descDSVShadowMap, sizeof(descDSVShadowMap));
	descDSVShadowMap.Format = DXGI_FORMAT_D32_FLOAT;
	descDSVShadowMap.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSVShadowMap.Texture2D.MipSlice = 0;

	if(FAILED(g_Device->CreateDepthStencilView(shadowMap, &descDSVShadowMap, &shadowMapDepthView)))
		return E_FAIL;

		
	// Create the shader-resource view from the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	
	if(FAILED(g_Device->CreateShaderResourceView(shadowMap, &srDesc, &shadowMapSRView)))
		return E_FAIL;


	ShadowMapViewPort.Width = (float)width;
	ShadowMapViewPort.Height = (float)height;
	ShadowMapViewPort.MinDepth = 0.0f;
	ShadowMapViewPort.MaxDepth = 1.0f;
	ShadowMapViewPort.TopLeftX = 0;
	ShadowMapViewPort.TopLeftY = 0;	
	
	return S_OK;
}

D3DXMATRIX& Light::GetViewProjMatrix()
{ 
	// DONT HAVE TO RECREATE UNLESS IT'S MOVED RIGHT?
	// Recreate LightViewProj
	D3DXMATRIX LightView;
	D3DXMatrixLookAtLH(&LightView, &this->pos, &this->lookAt, &this->up);
	D3DXMATRIX LightProj;
	D3DXMatrixPerspectiveFovLH(&LightProj, (float)D3DX_PI * 0.55f, 1, 5.0f, 100.0f);	////////////////////// NearClip NearPlane FarClip FarPlane
	this->ViewProj = LightView * LightProj;
	return this->ViewProj; 
}

void Light::SetLookAt(Vector3 la)
{
	this->lookAt = D3DXVECTOR3(la.x, la.y, la.z);
	Vector3 v(1.0f, 0.0f, 0.0f);
	Vector3 cp = v.GetCrossProduct(Vector3(la.x, la.y, la.z));
	this->up = D3DXVECTOR3(cp.x, cp.y, cp.z);
}

void Light::SetColor( Vector3 col )
{
	this->color = D3DXVECTOR3(col.x, col.y, col.z);
}

Vector3 Light::GetColor() const
{
	return Vector3(this->color.x, this->color.y, this->color.z);
}

void Light::SetPosition( Vector3 pos )
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}

Vector3 Light::GetPosition() const
{
	return Vector3(this->pos.x, pos.y, pos.z);
}

void Light::Move( Vector3 moveBy )
{
	this->pos += D3DXVECTOR3(moveBy.x, moveBy.y, moveBy.z);
}

void Light::SetUp( Vector3 up )
{
	this->up = D3DXVECTOR3(up.x, up.y, up.z);
}

void Light::SetIntensity( float intensity )
{
	this->intensity = intensity;
}

float Light::GetIntensity() const
{
	return this->intensity;
}

bool Light::IsUsingShadowMap() const
{
	return this->UseShadowMap;
}
