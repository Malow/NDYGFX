#ifndef RENDER_TARGET__H
#define RENDER_TARGET__H

#include "..\Source\BTHFBX\Include\stdafx.h"

class Device3D;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	HRESULT Initiate(Device3D* device, UINT width, UINT height, bool hasColorMap, 
		DXGI_FORMAT colorFormat, bool hasDepthMap);

	HRESULT Resize(UINT width, UINT height);

	ID3D11ShaderResourceView* GetColorSRV();
	ID3D11ShaderResourceView* GetDepthSRV();

	ID3D11RenderTargetView* GetColorRTV();
	ID3D11DepthStencilView* GetDepthSV();

	UINT GetWidth();
	UINT GetHeight();

	void Begin();
	void End();
	void Clear(float R, float G, float B, float A);

private:
	RenderTarget(const RenderTarget& rhs);
	RenderTarget& operator=(const RenderTarget& rhs);

	HRESULT CreateDepthMap();
	HRESULT CreateColorMap();
private:
	UINT mWidth;
	UINT mHeight;
	bool mHasColorMap, mHasDepthMap;
	DXGI_FORMAT mColorMapFormat;

	Device3D* mDevice3D;

	ID3D11ShaderResourceView* mColorMapSRV;
	ID3D11RenderTargetView* mColorMapRTV;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;
};

#endif // RenderTarget_H