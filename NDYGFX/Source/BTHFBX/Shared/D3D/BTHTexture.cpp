#include "BTHTexture.h"

BTHTexture::BTHTexture()
{
	m_pTexture = 0;
	m_pTextureView = 0;

	m_iTextureWidth = 0;
	m_iTextureHeight = 0;

	mDevice3D = NULL;
	mDeviceContextUsedToLock = NULL;
}

BTHTexture::~BTHTexture()
{
	Release();
}

HRESULT BTHTexture::Init(ID3D11Device* dev, ID3D11DeviceContext* devCont, int width, int height, bool mappable, void* initData)
{
	HRESULT hr = S_OK;

	// Create depth stencil texture and since we don't
	// need a stencil buffer we take 32-bits for depth buffer
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = mappable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = mappable ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initData;
	data.SysMemPitch = width * 4;

	hr = dev->CreateTexture2D( &desc, initData ? &data : NULL, &m_pTexture );
	if( FAILED( hr ) )
	{
		MessageBox(NULL, "Unable to create texture.", "Texture::Init(...) Error", MB_ICONERROR | MB_OK);
		return hr;
	}
	
	//init view description
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc; 
	ZeroMemory( &viewDesc, sizeof(viewDesc) ); 
	
	viewDesc.Format					= desc.Format;
	viewDesc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels	= desc.MipLevels;

	if(FAILED(hr = dev->CreateShaderResourceView( m_pTexture, &viewDesc, &m_pTextureView )))
	{
		MessageBox(0, "Unable to create shader resource view", "Error!", 0);
	}
	else
	{
		m_iTextureWidth = (float)width;
		m_iTextureHeight = (float)height;
	}

	return hr;
}

void BTHTexture::UpdateData(void* data, ID3D11DeviceContext* deviceContext, D3D11_BOX* pDestRegion )
{
	D3D11_BOX destRegion;
	if(pDestRegion == NULL)
	{
		// update texture resource with new pixel data
		destRegion.left = 0;
		destRegion.right = m_iTextureWidth;
		destRegion.top = 0;
		destRegion.bottom = m_iTextureHeight;
		destRegion.front = 0;
		destRegion.back = 1;

		pDestRegion = &destRegion;
	}

	if(deviceContext)
		deviceContext->UpdateSubresource(m_pTexture, 0, pDestRegion, data, pDestRegion->right * 4, 0);
}

HRESULT BTHTexture::Init(const std::string& fileName, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	zFileName = fileName;

	HRESULT hr = S_OK;

	ID3D11Resource* resource;
	if(FAILED(hr = D3DX11CreateTextureFromFile(dev, zFileName.c_str(), 0, 0, &resource, &hr)))
	{
		if(hr == D3D11_ERROR_FILE_NOT_FOUND)
		{
			/*
			char msg[100];
			sprintf_s(msg, sizeof(msg), "%s not found!", filename);
			MessageBox(0, msg, "Error!", 0);
			*/
		}

		return hr;
	}

	//convert resource to texture2D to be able to get dimensions
	resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_pTexture);

	//get texture information
	D3D11_TEXTURE2D_DESC desc; 
	m_pTexture->GetDesc( &desc ); 
	
	//init view description
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc; 
	ZeroMemory( &viewDesc, sizeof(viewDesc) ); 
	
	viewDesc.Format					= desc.Format;
	viewDesc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels	= desc.MipLevels;

	if(FAILED(hr = dev->CreateShaderResourceView( m_pTexture, &viewDesc, &m_pTextureView )))
	{
		MessageBox(0, "Unable to create shader resource view", "Error!", 0);
	}

	//load shader resource from file (NOT ABLE TO GET TEXTURE INFO WHEN USING THIS!!
	//hr = D3DX10CreateShaderResourceViewFromFile(device->GetDevice(),filename,NULL,NULL,&m_pTextureView,NULL);

	if(hr == S_OK)
	{
		m_iTextureWidth = (float)desc.Width;
		m_iTextureHeight = (float)desc.Height;
	}

	return hr;
}

void* BTHTexture::Map(ID3D11DeviceContext* deviceContext)
{
	void* ret = NULL;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	HRESULT hr = S_OK;

	mDeviceContextUsedToLock = deviceContext;
	if(FAILED(hr = mDeviceContextUsedToLock->Map(
		m_pTexture,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource)))
	{
		ret = NULL;
	}
	else
	{
		ret = MappedResource.pData;
	}

	return ret;
}

void BTHTexture::Unmap()
{
	if(mDeviceContextUsedToLock)
	{
		mDeviceContextUsedToLock->Unmap( m_pTexture, 0 );
		mDeviceContextUsedToLock = NULL;
	}
}

void BTHTexture::Release()
{
	//release memory
	if(m_pTextureView)
	{
		m_pTextureView->Release();
		m_pTextureView = NULL;
	}

	if(m_pTexture)
	{
		while(m_pTexture->Release());
		m_pTexture = NULL;
	}
}

ID3D11ShaderResourceView* BTHTexture::GetResource()
{
	return m_pTextureView;
}

float BTHTexture::GetWidth()
{
	return m_iTextureWidth;
}

float BTHTexture::GetHeight()
{
	return m_iTextureHeight;
}