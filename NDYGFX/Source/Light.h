#ifndef LIGHT
#define LIGHT

#include "DirectX.h"
#include "iLight.h"

enum LightType
{
	POINT_LIGHT = 0,
};

struct SunLight
{
	SunLight() 
	{ 
		this->direction = D3DXVECTOR3(0, -1, 0);
		this->lightColor = D3DXVECTOR3(1, 1, 1);
		this->intensity = 1.0f;
	}
	D3DXVECTOR3 direction;
	D3DXVECTOR3 lightColor;
	float intensity;
};

class Light : public iLight
{
private:
	D3DXVECTOR3 pos;
	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 up;

	LightType type;
	D3DXVECTOR3 color;
	float intensity;


	// Shadow Map
	bool UseShadowMap;

	int ShadowMapWidth;
	int ShadowMapHeight;
	D3DXMATRIX ViewProj;

	ID3D11Texture2D* shadowMap;
	ID3D11DepthStencilView* shadowMapDepthView;
	ID3D11ShaderResourceView* shadowMapSRView;
	D3D11_VIEWPORT ShadowMapViewPort;

public:
	Light();
	Light(D3DXVECTOR3 pos, LightType type = POINT_LIGHT);
	virtual ~Light();

	/* Accessible from outside */
	void SetColor(Vector3 col);
	Vector3 GetColor() const;
	void SetPosition(Vector3 pos);
	Vector3 GetPosition() const;

	void Move(Vector3 moveBy);
	void SetLookAt(Vector3 la);
	void SetUp(Vector3 up);
	void SetIntensity(float intensity);
	float GetIntensity() const;
	bool IsUsingShadowMap() const;

	/* Not accessible from outside */
	D3DXVECTOR3 GetColorD3DX() const { return this->color; }
	void SetColorD3DX(D3DXVECTOR3 col) { this->color = col; }
	D3DXVECTOR3 GetPositionD3DX() const { return this->pos; }
	void SetPositionD3DX(D3DXVECTOR3 pos) { this->pos = pos; }

	HRESULT InitShadowMap(ID3D11Device* g_Device, int quality);
	D3DXMATRIX& GetViewProjMatrix();
	void SetViewProjMatrix(D3DXMATRIX vp) { this->ViewProj = vp; }
	D3DXVECTOR3 GetLookAt() const { return this->lookAt; }
	D3DXVECTOR3 GetUp() const { return this->up; }
	D3D11_VIEWPORT& GetShadowMapViewPort() { return this->ShadowMapViewPort; }
	ID3D11ShaderResourceView* GetShadowMapSRV() const { return this->shadowMapSRView; }
	ID3D11DepthStencilView* GetShadowMapDSV() const { return this->shadowMapDepthView; }
	ID3D11Texture2D* GetShadowMap() const { return this->shadowMap; }
};



#endif