#include "Texture.h"

#include "Device3D.h"

Texture::Texture()
{
	m_pTexture = 0;
	m_pTextureView = 0;

	m_iTextureWidth = 0;
	m_iTextureHeight = 0;
	strcpy_s(m_cFilename, sizeof(m_cFilename), "");

	mDevice3D = NULL;
	mDeviceContextUsedToLock = NULL;
}

Texture::~Texture()
{
	Release();
}

HRESULT Texture::Init(Device3D* device, int width, int height, bool mappable, void* initData)
{
	HRESULT hr = S_OK;

	mDevice3D = device;

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

	hr = mDevice3D->GetDevice()->CreateTexture2D( &desc, initData ? &data : NULL, &m_pTexture );
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

	if(FAILED(hr = device->GetDevice()->CreateShaderResourceView( m_pTexture, &viewDesc, &m_pTextureView )))
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

void Texture::UpdateData(void* data, ID3D11DeviceContext* deviceContext, D3D11_BOX* pDestRegion )
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
	else
		mDevice3D->GetDeviceContext()->UpdateSubresource(m_pTexture, 0, pDestRegion, data, pDestRegion->right * 4, 0);
}

HRESULT Texture::Init(Device3D* device, const char* filename)
{
	HRESULT hr = S_OK;

	mDevice3D = device;

	strcpy_s(m_cFilename, sizeof(m_cFilename), filename);

	ID3D11Resource* resource;
	if(FAILED(hr = D3DX11CreateTextureFromFile(device->GetDevice(), filename, 0, 0, &resource, &hr)))
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

	if(FAILED(hr = device->GetDevice()->CreateShaderResourceView( m_pTexture, &viewDesc, &m_pTextureView )))
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

void* Texture::Map(ID3D11DeviceContext* deviceContext)
{
	void* ret = NULL;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	HRESULT hr = S_OK;

	mDeviceContextUsedToLock = deviceContext != NULL ? deviceContext : mDevice3D->GetDeviceContext();
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

void Texture::Unmap()
{
	if(mDeviceContextUsedToLock)
	{
		mDeviceContextUsedToLock->Unmap( m_pTexture, 0 );
		mDeviceContextUsedToLock = NULL;
	}
}

void Texture::Release()
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

ID3D11ShaderResourceView* Texture::GetResource()
{
	return m_pTextureView;
}

float Texture::GetWidth()
{
	return m_iTextureWidth;
}

float Texture::GetHeight()
{
	return m_iTextureHeight;
}

const char* Texture::GetFilename()
{
	return m_cFilename;
}