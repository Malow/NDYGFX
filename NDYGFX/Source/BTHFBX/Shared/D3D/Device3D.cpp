#include "Device3D.h"

Device3D::Device3D()
	: mSwapChain(NULL), mRenderTargetView(NULL), mDepthStencil(NULL),
	mDepthStencilView(NULL), mDevice(NULL),	mDeviceContext(NULL)
{
	mTexture2D = NULL;
	mBackbufferUAV = NULL;

	m_iWidth = 0;
	m_iHeight = 0;

	mMultisamplingUsed.Count = 1;
	mMultisamplingUsed.Quality = 0;
}

Device3D::~Device3D()
{
	if(mDeviceContext)
		mDeviceContext->ClearState();

	SAFE_RELEASE(mBackbufferUAV);
	SAFE_RELEASE(mSwapChain);
	SAFE_RELEASE(mTexture2D);
	SAFE_RELEASE(mRenderTargetView);
	SAFE_RELEASE(mDepthStencil);
	SAFE_RELEASE(mDepthStencilView);
	SAFE_RELEASE(mDeviceContext);
	SAFE_RELEASE(mDevice);
}

D3D_FEATURE_LEVEL Device3D::GetHighestFeatureLevel()
{
	HRESULT hr;

	//get maximum feature level
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_10_0;

	hr = D3D11CreateDevice(	NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		NULL,
		&FeatureLevel,
		NULL
		);

	return FeatureLevel;
}

HRESULT Device3D::ClearScreen(float r, float g, float b, float a)
{
	HRESULT hr = S_OK;

	float ClearColor[4] = {r, g, b, a};
	mDeviceContext->ClearRenderTargetView( mRenderTargetView, ClearColor );
	mDeviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );

	if(mDeviceContext && false)
	{
		//SAFE_RELEASE(m_pRenderTargetView);
		if(mRenderTargetView)
		{
			while(mRenderTargetView->Release());
			mRenderTargetView = NULL;
		}
		hr = mDevice->CreateRenderTargetView(mTexture2D, NULL, &mRenderTargetView);
		if(FAILED(hr))
		{
			MessageBox(NULL, "Unable to create render target view.", "Starfield Error", MB_ICONERROR | MB_OK);
		}
		mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

		float ClearColor[4] = {r, g, b, a};
		mDeviceContext->ClearRenderTargetView( mRenderTargetView, ClearColor );
		mDeviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );
	}

	return hr;
}

HRESULT Device3D::PresentScreen()
{
	if(FAILED(mSwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}

HRESULT Device3D::CreateTargetView()
{
	HRESULT hr = S_OK;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	// create shader unordered access view on back buffer for compute shader to write into texture
	hr = mDevice->CreateUnorderedAccessView( pBackBuffer, NULL, &mBackbufferUAV );

	SAFE_RELEASE(mRenderTargetView);
	hr = mDevice->CreateRenderTargetView( pBackBuffer, NULL, &mRenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;

	return hr;
}

ID3D11UnorderedAccessView* Device3D::GetBackbufferUAV()
{
	return mBackbufferUAV;
}

/*
HRESULT Device3D::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_iWidth;
	descDepth.Height = m_iHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mDevice->CreateTexture2D( &descDepth, NULL, &mDepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mDevice->CreateDepthStencilView( mDepthStencil, &descDSV, &mDepthStencilView );
	if( FAILED(hr) )
		return hr;

	return hr;
}
*/

HRESULT Device3D::Initiate(DEVICE_INIT_DATA data)
{
	HRESULT hr = S_OK;;
	
	RECT rc;
	GetClientRect( data.Hwnd, &rc );
	m_iWidth = rc.right - rc.left;;
	m_iHeight = rc.bottom - rc.top;

	m_hWnd = data.Hwnd;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_iWidth;
	sd.BufferDesc.Height = m_iHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	sd.OutputWindow = data.Hwnd;
	//sd.SampleDesc.Count = 4;
	//sd.SampleDesc.Quality = 8;
	sd.SampleDesc = mMultisamplingUsed;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&mSwapChain,
			&mDevice,
			&initiatedFeatureLevel,
			&mDeviceContext);

		if( SUCCEEDED( hr ) )
		{
			/*
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"BTH - Direct3D 11.0 Template | Direct3D 11.0 device initiated with Direct3D %s feature level",
				FeatureLevelToString(initiatedFeatureLevel)
			);
			SetWindowText(mhWnd, title);
			*/

			break;
		}
	}
	if( FAILED(hr) )
		return hr;

	if(FAILED(hr = CreateTargetView()))
		return hr;

	if(FAILED(hr = CreateDepthStencilView()))
		return hr;


	RestoreTargets();

	return hr;
}

DXGI_SAMPLE_DESC Device3D::GetMultisamplingUsed()
{
	return mMultisamplingUsed;
}

HRESULT Device3D::Resize(INT32 Width, INT32 Height)
{
	HRESULT hr = S_OK;

	m_iWidth = Width;
	m_iHeight = Height;

	/*
	SAFE_RELEASE(mTexture2D);
	SAFE_RELEASE(mRenderTargetView);
	SAFE_RELEASE(mDepthStencilView);
	*/

	if(mTexture2D)
	{
		while(mTexture2D->Release());
		mTexture2D = NULL;
	}

	if(mRenderTargetView)
	{
		while(mRenderTargetView->Release());
		mRenderTargetView = NULL;
	}

	if(mDepthStencilView)
	{
		while(mDepthStencilView->Release());
		mDepthStencilView = NULL;
	}

	if(FAILED(CreateTargetView()))
		return E_FAIL;

	if(FAILED(CreateDepthStencilView()))
		return E_FAIL;

	SetViewport();

	return hr;
}

HRESULT Device3D::CreateDepthStencilView()
{
	HRESULT hr;

	// Create depth stencil texture and since we don't
	// need a stencil buffer we take 32-bits for depth buffer
	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_iWidth;
	descDepth.Height = m_iHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	//descDepth.SampleDesc.Count = 4;
	//descDepth.SampleDesc.Quality = 8;
	descDepth.SampleDesc = mMultisamplingUsed;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
	if( FAILED( hr ) )
	{
		MessageBox(NULL, "Unable to create depth texture.", "Starfield Error", MB_ICONERROR | MB_OK);
		return hr;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;


	descDSV.Flags = 0;

	if( descDepth.SampleDesc.Count > 1 )
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	else
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &mDepthStencilView );
	if( FAILED( hr ) )
	{
		MessageBox(NULL, "Unable to create depthstencil view.", "Starfield Error", MB_ICONERROR | MB_OK);
		return hr;
	}

	//SAFE_RELEASE(pDepthStencil);
	if(pDepthStencil)
	{
		while(pDepthStencil->Release());
		pDepthStencil = NULL;
	}

	return hr;
}

ID3D11DepthStencilView* Device3D::GetDepthStencilView()
{
	return mDepthStencilView;
}

INT32 Device3D::GetWidth()
{
	return m_iWidth;
}

INT32 Device3D::GetHeight()
{
	return m_iHeight;
}

HWND Device3D::GetHWND()
{
	return m_hWnd;
}

void Device3D::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width		= (float)m_iWidth;
	vp.Height		= (float)m_iHeight;
	vp.MinDepth		= 0.0f;
	vp.MaxDepth		= 1.0f;
	vp.TopLeftX		= 0;
	vp.TopLeftY		= 0;
	mDeviceContext->RSSetViewports( 1, &vp );
}

ID3D11Device* Device3D::GetDevice()
{
	return mDevice;
}

ID3D11DeviceContext* Device3D::GetDeviceContext()
{
	return mDeviceContext;
}

ID3D11Texture2D* Device3D::GetRenderTarget()
{
	return mTexture2D;
}

ID3D11RenderTargetView* Device3D::GetBackbufferView()
{
	return mRenderTargetView;
}

void Device3D::RestoreTargets(bool restoreDepth)
{
	mDeviceContext->OMSetRenderTargets( 1, &mRenderTargetView, restoreDepth ? mDepthStencilView : NULL);

	SetViewport();
}