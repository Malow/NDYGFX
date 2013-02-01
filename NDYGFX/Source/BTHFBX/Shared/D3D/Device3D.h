/*
#ifndef DEVICE_3D__H
#define DEVICE_3D__H

#include "BTHFBX\Include\BTHstdafx.h"

struct DEVICE_INIT_DATA
{
	HWND Hwnd;
	INT32 Width;
	INT32 Height;
};

class Device3D
{
protected:
	//Direct3D device pointers
	IDXGISwapChain*				mSwapChain;
	ID3D11RenderTargetView*		mRenderTargetView;
	ID3D11Texture2D*			mDepthStencil;
	ID3D11DepthStencilView*		mDepthStencilView;
	ID3D11Device*				mDevice;
	ID3D11DeviceContext*		mDeviceContext;
	ID3D11UnorderedAccessView*  mBackbufferUAV;

	DXGI_SAMPLE_DESC			mMultisamplingUsed;

	//texture pointer used by Direc3D9Ex and Direct2D devices
	ID3D11Texture2D*			mTexture2D;

	//backbuffer dimensions
	INT32	m_iWidth;
	INT32	m_iHeight;
	HWND	m_hWnd;

protected:

	D3D_FEATURE_LEVEL GetHighestFeatureLevel();
	HRESULT CreateTargetView();
	HRESULT CreateDepthStencilView();
	void SetViewport();

public:
	Device3D();
	~Device3D();
	HRESULT Initiate(DEVICE_INIT_DATA data);
	HRESULT Resize(INT32 Width, INT32 Height);

	HRESULT ClearScreen(float r, float g, float b, float a);
	HRESULT PresentScreen();

	INT32 GetWidth();
	INT32 GetHeight();
	HWND GetHWND();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void RestoreTargets(bool restoreDepth = true);

	ID3D11Texture2D* GetRenderTarget();
	ID3D11DepthStencilView* GetDepthStencilView();
	ID3D11RenderTargetView* GetBackbufferView();
	ID3D11UnorderedAccessView* GetBackbufferUAV();

	DXGI_SAMPLE_DESC GetMultisamplingUsed();
};

#endif
*/