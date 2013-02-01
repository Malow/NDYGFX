#include "RenderTarget.h"
#include "..\Source\BTHFBX\Shared\D3D\Device3D.h"

RenderTarget::RenderTarget()
: mWidth(0), mHeight(0), mColorMapFormat(DXGI_FORMAT_UNKNOWN), 
  mDevice3D(0), mColorMapSRV(0), mColorMapRTV(0), mDepthMapSRV(0), mDepthMapDSV(0)
{
	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
}

RenderTarget::~RenderTarget()
{
	SAFE_RELEASE(mColorMapSRV);
	SAFE_RELEASE(mColorMapRTV);

	SAFE_RELEASE(mDepthMapSRV);
	SAFE_RELEASE(mDepthMapDSV);
}

HRESULT RenderTarget::Initiate(Device3D* device, UINT width, UINT height, bool hasColorMap, 
		                 DXGI_FORMAT colorFormat, bool hasDepthMap)
{
	mWidth  = width;
	mHeight = height;
	mHasColorMap = hasColorMap;
	mHasDepthMap = hasDepthMap;

	mColorMapFormat = colorFormat;

	mDevice3D = device;

	if( hasDepthMap)
	{
		if(FAILED(CreateDepthMap()))
			return E_FAIL;
	}

	// shadow maps don't need color maps, for example
	if( hasColorMap )
	{
		if(FAILED(CreateColorMap()))
			return E_FAIL;
	}
 
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width    = (float)width;
	mViewport.Height   = (float)height;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	return S_OK;
}

HRESULT RenderTarget::Resize(UINT width, UINT height)
{
	if(mDevice3D)
		return Initiate(mDevice3D, width, height, mHasColorMap, mColorMapFormat, mHasDepthMap);
	else
		return E_FAIL;
}

ID3D11ShaderResourceView* RenderTarget::GetColorSRV()
{
	return mColorMapSRV;
}

ID3D11ShaderResourceView* RenderTarget::GetDepthSRV()
{
	return mDepthMapSRV;
}

ID3D11RenderTargetView* RenderTarget::GetColorRTV()
{
	return mColorMapRTV;
}

ID3D11DepthStencilView* RenderTarget::GetDepthSV()
{
	return mDepthMapDSV;
}

void RenderTarget::Begin()
{
	ID3D11RenderTargetView* renderTargets[1] = {mColorMapRTV};
	mDevice3D->GetDeviceContext()->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);

	mDevice3D->GetDeviceContext()->RSSetViewports(1, &mViewport);

	//Clear(0,0,0,1);
}

void RenderTarget::Clear(float R, float G, float B, float A)
{
	// only clear is we actually created a color map.
	if( mColorMapRTV )
	{
		float ClearColor[4] = {R, G, B, A};
		mDevice3D->GetDeviceContext()->ClearRenderTargetView(mColorMapRTV, ClearColor);
	}

	if(mDepthMapDSV)
	{
		mDevice3D->GetDeviceContext()->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
}

void RenderTarget::End()
{
	// After we have drawn to the color map, have the hardware generate
	// the lower mipmap levels.

	if( mColorMapSRV )
		mDevice3D->GetDeviceContext()->GenerateMips(mColorMapSRV);

	mDevice3D->RestoreTargets();
}	

HRESULT RenderTarget::CreateDepthMap()
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* depthMap = 0;

	SAFE_RELEASE(mDepthMapSRV);
	SAFE_RELEASE(mDepthMapDSV);

	D3D11_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = mWidth;
	texDesc.Height    = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;

	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	if(FAILED(hr = mDevice3D->GetDevice()->CreateTexture2D(&texDesc, 0, &depthMap)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	dsvDesc.Texture2D.MipSlice = 0;
	if(FAILED(hr = mDevice3D->GetDevice()->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV)))
		return E_FAIL;


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	if(FAILED(hr = mDevice3D->GetDevice()->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV)))
		return E_FAIL;

	// View saves a reference to the texture so we can release our reference.
	SAFE_RELEASE(depthMap);

	return S_OK;
}

HRESULT RenderTarget::CreateColorMap()
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* colorMap = 0;

	SAFE_RELEASE(mColorMapRTV);
	SAFE_RELEASE(mColorMapSRV);

	D3D11_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = mWidth;
	texDesc.Height    = mHeight;
	texDesc.ArraySize = 1;
	texDesc.Format    = mColorMapFormat;
	texDesc.SampleDesc.Count = 1;  
	texDesc.SampleDesc.Quality = 0;
	texDesc.MipLevels = 0;

	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if(FAILED(hr = mDevice3D->GetDevice()->CreateTexture2D(&texDesc, 0, &colorMap)))
		return E_FAIL;



	D3D11_RENDER_TARGET_VIEW_DESC rvd;
	rvd.Format = texDesc.Format;
	rvd.Texture2D.MipSlice = 0;
	rvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	// Null description means to create a view to all mipmap levels using 
	// the format the texture was created with.
	if(FAILED(hr = mDevice3D->GetDevice()->CreateRenderTargetView(colorMap, &rvd, &mColorMapRTV)))
		return E_FAIL;



	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format = texDesc.Format;
	srv.Texture2D.MostDetailedMip = 0;
	srv.Texture2D.MipLevels = 1;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	if(FAILED(hr = mDevice3D->GetDevice()->CreateShaderResourceView(colorMap, &srv, &mColorMapSRV)))
		return E_FAIL;

	// View saves a reference to the texture so we can release our reference.
	SAFE_RELEASE(colorMap);

	return S_OK;
}

UINT RenderTarget::GetWidth()
{
	return mWidth;
}

UINT RenderTarget::GetHeight()
{
	return mHeight;
}